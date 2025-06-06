#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tlsf.h"
#include "tlsfbits.h"

/*
** Constants.
*/

/* Public constants: may be modified. */
enum tlsf_public
{
	/* log2 of number of linear subdivisions of block sizes. */
	SL_INDEX_COUNT_LOG2 = 5,
};

/* Private constants: do not modify. */
enum tlsf_private
{
#if defined (TLSF_64BIT)
	/* All allocation sizes and addresses are aligned to 8 bytes. */
	ALIGN_SIZE_LOG2 = 3,
#else
	/* All allocation sizes and addresses are aligned to 4 bytes. */
	ALIGN_SIZE_LOG2 = 2,
#endif
	ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

	/*
	** We support allocations of sizes up to (1 << FL_INDEX_MAX) bits.
	** However, because we linearly subdivide the second-level lists, and
	** our minimum size granularity is 4 bytes, it doesn't make sense to
	** create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
	** or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
	** trying to split size ranges into more slots than we have available.
	** Instead, we calculate the minimum threshold size, and place all
	** blocks below that size into the 0th first-level list.
	*/

#if defined (TLSF_64BIT)
	/*
	** TODO: We can increase this to support larger sizes, at the expense
	** of more overhead in the TLSF structure.
	*/
	FL_INDEX_MAX = 32,
#else
#  if defined(CAFE)
	FL_INDEX_MAX = 31, // Wii U needs 1.8 GB of pool on devkit
#  else
	FL_INDEX_MAX = 30,
#  endif
#endif
	SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
	FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
	FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

	SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};

/*
** Cast and min/max macros.
*/

#define tlsf_cast(t, exp)	((t) (exp))
#define tlsf_min(a, b)		((a) < (b) ? (a) : (b))
#define tlsf_max(a, b)		((a) > (b) ? (a) : (b))

/*
** Set assert macro, if it has not been provided by the user.
*/
#if !defined (tlsf_assert)
#if defined(__flash__)
//extern void tlsf_assert(int condition);
#define tlsf_assert
#else
#define tlsf_assert assert
#endif
#endif

/*
** Static assertion mechanism.
*/

#define _tlsf_glue2(x, y) x ## y
#define _tlsf_glue(x, y) _tlsf_glue2(x, y)
#define tlsf_static_assert(exp) \
	typedef char _tlsf_glue(static_assert, __LINE__) [(exp) ? 1 : -1]

/* This code has been tested on 32- and 64-bit (LP/LLP) architectures. */
tlsf_static_assert(sizeof(int)* CHAR_BIT == 32);
tlsf_static_assert(sizeof(size_t)* CHAR_BIT >= 32);
tlsf_static_assert(sizeof(size_t)* CHAR_BIT <= 64);

/* SL_INDEX_COUNT must be <= number of bits in sl_bitmap's storage type. */
tlsf_static_assert(sizeof(unsigned int)* CHAR_BIT >= SL_INDEX_COUNT);

/* Ensure we've properly tuned our sizes. */
tlsf_static_assert(ALIGN_SIZE == SMALL_BLOCK_SIZE / SL_INDEX_COUNT);

/*
** Data structures and associated constants.
*/

/*
** Block header structure.
**
** There are several implementation subtleties involved:
** - The prev_phys_block field is only valid if the previous block is free.
** - The prev_phys_block field is actually stored at the end of the
**   previous block. It appears at the beginning of this structure only to
**   simplify the implementation.
** - The next_free / prev_free fields are only valid if the block is free.
*/
typedef struct block_header_t
{
	/* Points to the previous physical block. */
	struct block_header_t* prev_phys_block;

	/* The size of this block, excluding the block header. */
	size_t size;

	/* Next and previous free blocks. */
	struct block_header_t* next_free;
	struct block_header_t* prev_free;
} block_header_t;

/*
** Since block sizes are always at least a multiple of 4, the two least
** significant bits of the size field are used to store the block status:
** - bit 0: whether block is busy or free
** - bit 1: whether previous block is busy or free
*/
static const size_t block_header_free_bit = 1 << 0;
static const size_t block_header_prev_free_bit = 1 << 1;

/*
** The size of the block header exposed to used blocks is the size field.
** The prev_phys_block field is stored *inside* the previous free block.
*/
static const size_t block_header_overhead = sizeof(size_t);

/* User data starts directly after the size field in a used block. */
static const size_t block_start_offset =
offsetof(block_header_t, size) + sizeof(size_t);

/*
** A free block must be large enough to store its header minus the size of
** the prev_phys_block field, and no larger than the number of addressable
** bits for FL_INDEX.
*/
static const size_t block_size_min =
sizeof(block_header_t) - sizeof(block_header_t*);
static const size_t block_size_max = tlsf_cast(size_t, 1) << FL_INDEX_MAX;


/* The TLSF pool structure. */
typedef struct pool_t
{
	/* Empty lists point at this block to indicate they are free. */
	block_header_t block_null;

	/* Bitmaps for free lists. */
	unsigned int fl_bitmap;
	unsigned int sl_bitmap[FL_INDEX_COUNT];

	/* Head of free lists. */
	block_header_t* blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];
} pool_t;

/* A type used for casting when doing pointer arithmetic. */
typedef ptrdiff_t tlsfptr_t;

/*
** block_header_t member functions.
*/

static size_t block_size(const block_header_t* block)
{
	return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static void block_set_size(block_header_t* block, size_t size)
{
	const size_t oldsize = block->size;
	block->size = size | (oldsize & (block_header_free_bit | block_header_prev_free_bit));
}

static int block_is_last(const block_header_t* block)
{
	return 0 == block_size(block);
}

static int block_is_free(const block_header_t* block)
{
	return tlsf_cast(int, block->size & block_header_free_bit);
}

static void block_set_free(block_header_t* block)
{
	block->size |= block_header_free_bit;
}

static void block_set_used(block_header_t* block)
{
	block->size &= ~block_header_free_bit;
}

static int block_is_prev_free(const block_header_t* block)
{
	return tlsf_cast(int, block->size & block_header_prev_free_bit);
}

static void block_set_prev_free(block_header_t* block)
{
	block->size |= block_header_prev_free_bit;
}

static void block_set_prev_used(block_header_t* block)
{
	block->size &= ~block_header_prev_free_bit;
}

static block_header_t* block_from_ptr(const void* ptr)
{
	return tlsf_cast(block_header_t*,
		tlsf_cast(unsigned char*, ptr) - block_start_offset);
}

static void* block_to_ptr(const block_header_t* block)
{
	return tlsf_cast(void*,
		tlsf_cast(unsigned char*, block) + block_start_offset);
}

/* Return location of next block after block of given size. */
static block_header_t* offset_to_block(const void* ptr, size_t size)
{
	return tlsf_cast(block_header_t*, tlsf_cast(tlsfptr_t, ptr) + size);
}

/* Return location of previous block. */
static block_header_t* block_prev(const block_header_t* block)
{
	return block->prev_phys_block;
}

/* Return location of next existing block. */
static block_header_t* block_next(const block_header_t* block)
{
	block_header_t* next = offset_to_block(block_to_ptr(block),
		block_size(block) - block_header_overhead);
	tlsf_assert(!block_is_last(block));
	return next;
}

/* Link a new block with its physical neighbor, return the neighbor. */
static block_header_t* block_link_next(block_header_t* block)
{
	block_header_t* next = block_next(block);
	next->prev_phys_block = block;
	return next;
}

static void block_mark_as_free(block_header_t* block)
{
	/* Link the block to the next block, first. */
	block_header_t* next = block_link_next(block);
	block_set_prev_free(next);
	block_set_free(block);
}

static void block_mark_as_used(block_header_t* block)
{
	block_header_t* next = block_next(block);
	block_set_prev_used(next);
	block_set_used(block);
}

static size_t align_up(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return (x + (align - 1)) & ~(align - 1);
}

static size_t align_down(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return x - (x & (align - 1));
}

static void* align_ptr(const void* ptr, size_t align)
{
	const tlsfptr_t aligned =
		(tlsf_cast(tlsfptr_t, ptr) + (align - 1)) & ~(align - 1);
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return tlsf_cast(void*, aligned);
}

/*
** Adjust an allocation size to be aligned to word size, and no smaller
** than internal minimum.
*/
static size_t adjust_request_size(size_t size, size_t align)
{
	size_t adjust = 0;
	if (size && size < block_size_max)
	{
		const size_t aligned = align_up(size, align);
		adjust = tlsf_max(aligned, block_size_min);
	}
	return adjust;
}

/*
** TLSF utility functions. In most cases, these are direct translations of
** the documentation found in the white paper.
*/

static void mapping_insert(size_t size, int* fli, int* sli)
{
	int fl, sl;
	if (size < SMALL_BLOCK_SIZE)
	{
		/* Store small blocks in first list. */
		fl = 0;
		sl = tlsf_cast(int, size) / (SMALL_BLOCK_SIZE / SL_INDEX_COUNT);
	}
	else
	{
		fl = tlsf_fls_sizet(size);
		sl = tlsf_cast(int, size >> (fl - SL_INDEX_COUNT_LOG2)) ^ (1 << SL_INDEX_COUNT_LOG2);
		fl -= (FL_INDEX_SHIFT - 1);
	}
	*fli = fl;
	*sli = sl;
}

/* This version rounds up to the next block size (for allocations) */
static void mapping_search(size_t size, int* fli, int* sli)
{
	if (size >= (1 << SL_INDEX_COUNT_LOG2))
	{
		const size_t round = (1 << (tlsf_fls_sizet(size) - SL_INDEX_COUNT_LOG2)) - 1;
		size += round;
	}
	mapping_insert(size, fli, sli);
}

static block_header_t* search_suitable_block(pool_t* pool, int* fli, int* sli)
{
	int fl = *fli;
	int sl = *sli;

	/*
	** First, search for a block in the list associated with the given
	** fl/sl index.
	*/
	unsigned int sl_map = pool->sl_bitmap[fl] & (~0 << sl);
	if (!sl_map)
	{
		/* No block exists. Search in the next largest first-level list. */
		const unsigned int fl_map = pool->fl_bitmap & (~0 << (fl + 1));
		if (!fl_map)
		{
			/* No free blocks available, memory has been exhausted. */
			return 0;
		}

		fl = tlsf_ffs(fl_map);
		*fli = fl;
		sl_map = pool->sl_bitmap[fl];
	}
	tlsf_assert(sl_map && "internal error - second level bitmap is null");
	sl = tlsf_ffs(sl_map);
	*sli = sl;

	/* Return the first block in the free list. */
	return pool->blocks[fl][sl];
}

/* Remove a free block from the free list.*/
static void remove_free_block(pool_t* pool, block_header_t* block, int fl, int sl)
{
	block_header_t* prev = block->prev_free;
	block_header_t* next = block->next_free;
	tlsf_assert(prev && "prev_free field can not be null");
	tlsf_assert(next && "next_free field can not be null");
	next->prev_free = prev;
	prev->next_free = next;

	/* If this block is the head of the free list, set new head. */
	if (pool->blocks[fl][sl] == block)
	{
		pool->blocks[fl][sl] = next;

		/* If the new head is null, clear the bitmap. */
		if (next == &pool->block_null)
		{
			pool->sl_bitmap[fl] &= ~(1 << sl);

			/* If the second bitmap is now empty, clear the fl bitmap. */
			if (!pool->sl_bitmap[fl])
			{
				pool->fl_bitmap &= ~(1 << fl);
			}
		}
	}
}

/* Insert a free block into the free block list. */
static void insert_free_block(pool_t* pool, block_header_t* block, int fl, int sl)
{
	block_header_t* current = pool->blocks[fl][sl];
	tlsf_assert(current && "free list cannot have a null entry");
	tlsf_assert(block && "cannot insert a null entry into the free list");
	block->next_free = current;
	block->prev_free = &pool->block_null;
	current->prev_free = block;

	tlsf_assert(block_to_ptr(block) == align_ptr(block_to_ptr(block), ALIGN_SIZE)
		&& "block not aligned properly");
	/*
	** Insert the new block at the head of the list, and mark the first-
	** and second-level bitmaps appropriately.
	*/
	pool->blocks[fl][sl] = block;
	pool->fl_bitmap |= (1 << fl);
	pool->sl_bitmap[fl] |= (1 << sl);
}

/* Remove a given block from the free list. */
static void block_remove(pool_t* pool, block_header_t* block)
{
	int fl, sl;
	mapping_insert(block_size(block), &fl, &sl);
	remove_free_block(pool, block, fl, sl);
}

/* Insert a given block into the free list. */
static void block_insert(pool_t* pool, block_header_t* block)
{
	int fl, sl;
	mapping_insert(block_size(block), &fl, &sl);
	insert_free_block(pool, block, fl, sl);
}

static int block_can_split(block_header_t* block, size_t size)
{
	return block_size(block) >= sizeof(block_header_t) + size;
}

/* Split a block into two, the second of which is free. */
static block_header_t* block_split(block_header_t* block, size_t size)
{
	/* Calculate the amount of space left in the remaining block. */
	block_header_t* remaining =
		offset_to_block(block_to_ptr(block), size - block_header_overhead);

	const size_t remain_size = block_size(block) - (size + block_header_overhead);

	tlsf_assert(block_to_ptr(remaining) == align_ptr(block_to_ptr(remaining), ALIGN_SIZE)
		&& "remaining block not aligned properly");

	tlsf_assert(block_size(block) == remain_size + size + block_header_overhead);
	block_set_size(remaining, remain_size);
	tlsf_assert(block_size(remaining) >= block_size_min && "block split with invalid size");

	block_set_size(block, size);
	block_mark_as_free(remaining);

	return remaining;
}

/* Absorb a free block's storage into an adjacent previous free block. */
static block_header_t* block_absorb(block_header_t* prev, block_header_t* block)
{
	tlsf_assert(!block_is_last(prev) && "previous block can't be last!");
	/* Note: Leaves flags untouched. */
	prev->size += block_size(block) + block_header_overhead;
	block_link_next(prev);
	return prev;
}

/* Merge a just-freed block with an adjacent previous free block. */
static block_header_t* block_merge_prev(pool_t* pool, block_header_t* block)
{
	if (block_is_prev_free(block))
	{
		block_header_t* prev = block_prev(block);
		tlsf_assert(prev && "prev physical block can't be null");
		tlsf_assert(block_is_free(prev) && "prev block is not free though marked as such");
		block_remove(pool, prev);
		block = block_absorb(prev, block);
	}

	return block;
}

/* Merge a just-freed block with an adjacent free block. */
static block_header_t* block_merge_next(pool_t* pool, block_header_t* block)
{
	block_header_t* next = block_next(block);
	tlsf_assert(next && "next physical block can't be null");

	if (block_is_free(next))
	{
		tlsf_assert(!block_is_last(block) && "previous block can't be last!");
		block_remove(pool, next);
		block = block_absorb(block, next);
	}

	return block;
}

/* Trim any trailing block space off the end of a block, return to pool. */
static void block_trim_free(pool_t* pool, block_header_t* block, size_t size)
{
	tlsf_assert(block_is_free(block) && "block must be free");
	if (block_can_split(block, size))
	{
		block_header_t* remaining_block = block_split(block, size);
		block_link_next(block);
		block_set_prev_free(remaining_block);
		block_insert(pool, remaining_block);
	}
}

/* Trim any trailing block space off the end of a used block, return to pool. */
static void block_trim_used(pool_t* pool, block_header_t* block, size_t size)
{
	tlsf_assert(!block_is_free(block) && "block must be used");
	if (block_can_split(block, size))
	{
		/* If the next block is free, we must coalesce. */
		block_header_t* remaining_block = block_split(block, size);
		block_set_prev_used(remaining_block);

		remaining_block = block_merge_next(pool, remaining_block);
		block_insert(pool, remaining_block);
	}
}

static block_header_t* block_trim_free_leading(pool_t* pool, block_header_t* block, size_t size)
{
	block_header_t* remaining_block = block;
	if (block_can_split(block, size))
	{
		/* We want the 2nd block. */
		remaining_block = block_split(block, size - block_header_overhead);
		block_set_prev_free(remaining_block);

		block_link_next(block);
		block_insert(pool, block);
	}

	return remaining_block;
}

static block_header_t* block_locate_free(pool_t* pool, size_t size)
{
	int fl = 0, sl = 0;
	block_header_t* block = 0;

	if (size)
	{
		mapping_search(size, &fl, &sl);
		block = search_suitable_block(pool, &fl, &sl);
	}

	if (block)
	{
		tlsf_assert(block_size(block) >= size);
		remove_free_block(pool, block, fl, sl);
	}

	return block;
}

static void* block_prepare_used(pool_t* pool, block_header_t* block, size_t size)
{
	void* p = 0;
	if (block)
	{
		block_trim_free(pool, block, size);
		block_mark_as_used(block);
		p = block_to_ptr(block);
	}
	return p;
}

/* Clear structure and point all empty lists at the null block. */
static void pool_construct(pool_t* pool)
{
	int i, j;

	pool->block_null.next_free = &pool->block_null;
	pool->block_null.prev_free = &pool->block_null;

	pool->fl_bitmap = 0;
	for (i = 0; i < FL_INDEX_COUNT; ++i)
	{
		pool->sl_bitmap[i] = 0;
		for (j = 0; j < SL_INDEX_COUNT; ++j)
		{
			pool->blocks[i][j] = &pool->block_null;
		}
	}
}

/*
** Debugging utilities.
*/

typedef struct integrity_t
{
	int prev_status;
	int status;
} integrity_t;

#define tlsf_insist(x) { tlsf_assert(x); if (!(x)) { status--; } }

static void integrity_walker(void* ptr, size_t size, int used, void* user)
{
	block_header_t* block = block_from_ptr(ptr);
	integrity_t* integ = tlsf_cast(integrity_t*, user);
	const int this_prev_status = block_is_prev_free(block) ? 1 : 0;
	const int this_status = block_is_free(block) ? 1 : 0;
	const size_t this_block_size = block_size(block);

	int status = 0;
	tlsf_insist(integ->prev_status == this_prev_status && "prev status incorrect");
	tlsf_insist(size == this_block_size && "block size incorrect");

	integ->prev_status = this_status;
	integ->status += status;
}

int tlsf_check_heap(tlsf_pool tlsf)
{
	int i, j;

	pool_t* pool = tlsf_cast(pool_t*, tlsf);
	int status = 0;

	/* Check that the blocks are physically correct. */
	integrity_t integ = { 0, 0 };
	tlsf_walk_heap(tlsf, integrity_walker, &integ);
	status = integ.status;

	/* Check that the free lists and bitmaps are accurate. */
	for (i = 0; i < FL_INDEX_COUNT; ++i)
	{
		for (j = 0; j < SL_INDEX_COUNT; ++j)
		{
			const int fl_map = pool->fl_bitmap & (1 << i);
			const int sl_list = pool->sl_bitmap[i];
			const int sl_map = sl_list & (1 << j);
			const block_header_t* block = pool->blocks[i][j];

			/* Check that first- and second-level lists agree. */
			if (!fl_map)
			{
				tlsf_insist(!sl_map && "second-level map must be null");
			}

			if (!sl_map)
			{
				tlsf_insist(block == &pool->block_null && "block list must be null");
				continue;
			}

			/* Check that there is at least one free block. */
			tlsf_insist(sl_list && "no free blocks in second-level map");
			tlsf_insist(block != &pool->block_null && "block should not be null");

			while (block != &pool->block_null)
			{
				int fli, sli;
				tlsf_insist(block_is_free(block) && "block should be free");
				tlsf_insist(!block_is_prev_free(block) && "blocks should have coalesced");
				tlsf_insist(!block_is_free(block_next(block)) && "blocks should have coalesced");
				tlsf_insist(block_is_prev_free(block_next(block)) && "block should be free");
				tlsf_insist(block_size(block) >= block_size_min && "block not minimum size");

				mapping_insert(block_size(block), &fli, &sli);
				tlsf_insist(fli == i && sli == j && "block size indexed in wrong list");
				block = block->next_free;
			}
		}
	}

	return status;
}

#undef tlsf_insist

static void default_walker(void* ptr, size_t size, int used, void* user)
{
	(void)user;
	printf("\t%p %s size: %x (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, block_from_ptr(ptr));
}

void tlsf_walk_heap(tlsf_pool pool, tlsf_walker walker, void* user)
{
	tlsf_walker heap_walker = walker ? walker : default_walker;
	block_header_t* block =
		offset_to_block(pool, sizeof(pool_t) - block_header_overhead);

	while (block && !block_is_last(block))
	{
		heap_walker(
			block_to_ptr(block),
			block_size(block),
			!block_is_free(block),
			user);
		block = block_next(block);
	}
}

size_t tlsf_block_size(void* ptr)
{
	size_t size = 0;
	if (ptr)
	{
		const block_header_t* block = block_from_ptr(ptr);
		size = block_size(block);
	}
	return size;
}

/*
** Overhead of the TLSF structures in a given memory block passed to
** tlsf_create, equal to the size of a pool_t plus overhead of the initial
** free block and the sentinel block.
*/
size_t tlsf_overhead()
{
	const size_t pool_overhead = sizeof(pool_t) + 2 * block_header_overhead;
	return pool_overhead;
}

/*
** TLSF main interface. Right out of the white paper.
*/

tlsf_pool tlsf_create(void* mem, size_t bytes)
{
	block_header_t* block;
	block_header_t* next;

	const size_t pool_overhead = tlsf_overhead();
	const size_t pool_bytes = align_down(bytes - pool_overhead, ALIGN_SIZE);
	pool_t* pool = tlsf_cast(pool_t*, mem);

#if _DEBUG
	/* Verify ffs/fls work properly. */
	int rv = 0;
	rv += (tlsf_ffs(0) == -1) ? 0 : 0x1;
	rv += (tlsf_fls(0) == -1) ? 0 : 0x2;
	rv += (tlsf_ffs(1) == 0) ? 0 : 0x4;
	rv += (tlsf_fls(1) == 0) ? 0 : 0x8;
	rv += (tlsf_ffs(0x80000000) == 31) ? 0 : 0x10;
	rv += (tlsf_ffs(0x80008000) == 15) ? 0 : 0x20;
	rv += (tlsf_fls(0x80000008) == 31) ? 0 : 0x40;
	rv += (tlsf_fls(0x7FFFFFFF) == 30) ? 0 : 0x80;

#if defined (TLSF_64BIT)
	rv += (tlsf_fls_sizet(0x80000000) == 31) ? 0 : 0x100;
	rv += (tlsf_fls_sizet(0x100000000) == 32) ? 0 : 0x200;
	rv += (tlsf_fls_sizet(0xffffffffffffffff) == 63) ? 0 : 0x400;
	if (rv)
	{
		printf("tlsf_create: %x ffs/fls tests failed!\n", rv);
		return 0;
	}
#endif
#endif

	if (pool_bytes < block_size_min || pool_bytes > block_size_max)
	{
#if defined (TLSF_64BIT)
		printf("tlsf_create: Pool size must be at least %d bytes.\n",
			(unsigned int)(pool_overhead + block_size_min));
#else
		printf("tlsf_create: Pool size must be between %u and %u bytes.\n",
			(unsigned int)(pool_overhead + block_size_min),
			(unsigned int)(pool_overhead + block_size_max));
#endif
		return 0;
	}

	/* Construct a valid pool object. */
	pool_construct(pool);

	/*
	** Create the main free block. Offset the start of the block slightly
	** so that the prev_phys_block field falls inside of the pool
	** structure - it will never be used.
	*/
	block = offset_to_block(
		tlsf_cast(void*, pool), sizeof(pool_t) - block_header_overhead);
	block_set_size(block, pool_bytes);
	block_set_free(block);
	block_set_prev_used(block);
	block_insert(pool, block);

	/* Split the block to create a zero-size pool sentinel block. */
	next = block_link_next(block);
	block_set_size(next, 0);
	block_set_used(next);
	block_set_prev_free(next);

	return tlsf_cast(tlsf_pool, pool);
}

void tlsf_destroy(tlsf_pool pool)
{
	/* Nothing to do. */
	(void)pool;
}

void* tlsf_malloc(tlsf_pool tlsf, size_t size)
{
	pool_t* pool = tlsf_cast(pool_t*, tlsf);
	const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
	block_header_t* block = block_locate_free(pool, adjust);
	return block_prepare_used(pool, block, adjust);
}

void* tlsf_memalign(tlsf_pool tlsf, size_t align, size_t size)
{
	pool_t* pool = tlsf_cast(pool_t*, tlsf);
	const size_t adjust = adjust_request_size(size, ALIGN_SIZE);

	/*
	** We must allocate an additional minimum block size bytes so that if
	** our free block will leave an alignment gap which is smaller, we can
	** trim a leading free block and release it back to the heap. We must
	** do this because the previous physical block is in use, therefore
	** the prev_phys_block field is not valid, and we can't simply adjust
	** the size of that block.
	*/
	const size_t gap_minimum = sizeof(block_header_t);
	const size_t size_with_gap = adjust_request_size(adjust + align + gap_minimum, align);

	/* If alignment is less than or equals base alignment, we're done. */
	const size_t aligned_size = (align <= ALIGN_SIZE) ? adjust : size_with_gap;

	block_header_t* block = block_locate_free(pool, aligned_size);

	/* This can't be a static assert. */
	tlsf_assert(sizeof(block_header_t) == block_size_min + block_header_overhead);

	if (block)
	{
		void* ptr = block_to_ptr(block);
		void* aligned = align_ptr(ptr, align);
		size_t gap = tlsf_cast(size_t,
			tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));

		/* If gap size is too small, offset to next aligned boundary. */
		if (gap && gap < gap_minimum)
		{
			const size_t gap_remain = gap_minimum - gap;
			const size_t offset = tlsf_max(gap_remain, align);
			const void* next_aligned = tlsf_cast(void*,
				tlsf_cast(tlsfptr_t, aligned) + offset);

			aligned = align_ptr(next_aligned, align);
			gap = tlsf_cast(size_t,
				tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));
		}

		if (gap)
		{
			tlsf_assert(gap >= gap_minimum && "gap size too small");
			block = block_trim_free_leading(pool, block, gap);
		}
	}

	return block_prepare_used(pool, block, adjust);
}

void tlsf_free(tlsf_pool tlsf, void* ptr)
{
	/* Don't attempt to free a NULL pointer. */
	if (ptr)
	{
		pool_t* pool = tlsf_cast(pool_t*, tlsf);
		block_header_t* block = block_from_ptr(ptr);
		block_mark_as_free(block);
		block = block_merge_prev(pool, block);
		block = block_merge_next(pool, block);
		block_insert(pool, block);
	}
}

/*
** The TLSF block information provides us with enough information to
** provide a reasonably intelligent implementation of realloc, growing or
** shrinking the currently allocated block as required.
**
** This routine handles the somewhat esoteric edge cases of realloc:
** - a non-zero size with a null pointer will behave like malloc
** - a zero size with a non-null pointer will behave like free
** - a request that cannot be satisfied will leave the original buffer
**   untouched
** - an extended buffer size will leave the newly-allocated area with
**   contents undefined
*/
void* tlsf_realloc(tlsf_pool tlsf, void* ptr, size_t size)
{
	pool_t* pool = tlsf_cast(pool_t*, tlsf);
	void* p = 0;

	/* Zero-size requests are treated as free. */
	if (ptr && size == 0)
	{
		tlsf_free(tlsf, ptr);
	}
	/* Requests with NULL pointers are treated as malloc. */
	else if (!ptr)
	{
		p = tlsf_malloc(tlsf, size);
	}
	else
	{
		block_header_t* block = block_from_ptr(ptr);
		block_header_t* next = block_next(block);

		const size_t cursize = block_size(block);
		const size_t combined = cursize + block_size(next) + block_header_overhead;
		const size_t adjust = adjust_request_size(size, ALIGN_SIZE);

		/*
		** If the next block is used, or when combined with the current
		** block, does not offer enough space, we must reallocate and copy.
		*/
		if (adjust > cursize && (!block_is_free(next) || adjust > combined))
		{
			p = tlsf_malloc(tlsf, size);
			if (p)
			{
				const size_t minsize = tlsf_min(cursize, size);
				memcpy(p, ptr, minsize);
				tlsf_free(tlsf, ptr);
			}
		}
		else
		{
			/* Do we need to expand to the next block? */
			if (adjust > cursize)
			{
				block_merge_next(pool, block);
				block_mark_as_used(block);
			}

			/* Trim the resulting block and return the original pointer. */
			block_trim_used(pool, block, adjust);
			p = ptr;
		}
	}

	return p;
}

void* tlsf_realloc_align(tlsf_pool tlsf, void* ptr, size_t align, size_t size)
{
	pool_t* pool = tlsf_cast(pool_t*, tlsf);
	void* p = 0;

	/* Zero-size requests are treated as free. */
	if (ptr && size == 0)
	{
		tlsf_free(tlsf, ptr);
	}
	/* Requests with NULL pointers are treated as malloc. */
	else if (!ptr)
	{
		p = tlsf_memalign(tlsf, align, size);
	}
	else
	{
		block_header_t* block = block_from_ptr(ptr);
		block_header_t* next = block_next(block);

		const size_t cursize = block_size(block);
		const size_t combined = cursize + block_size(next) + block_header_overhead;
		const size_t adjust = adjust_request_size(size, ALIGN_SIZE);

		/*
		** If the next block is used, or when combined with the current
		** block, does not offer enough space, we must reallocate and copy.
		*/
		if ((adjust > cursize && (!block_is_free(next) || adjust > combined)) ||
			align_ptr(ptr, align) != ptr)
		{
			p = tlsf_memalign(tlsf, align, size);
			if (p)
			{
				const size_t minsize = tlsf_min(cursize, size);
				memcpy(p, ptr, minsize);
				tlsf_free(tlsf, ptr);
			}
		}
		else
		{
			/* Do we need to expand to the next block? */
			if (adjust > cursize)
			{
				block_merge_next(pool, block);
				block_mark_as_used(block);
			}

			/* Trim the resulting block and return the original pointer. */
			block_trim_used(pool, block, adjust);
			p = ptr;
		}
	}

	return p;
}
