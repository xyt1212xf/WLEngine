#pragma once
#include "AllocatorLabels.h"

//extern void* operator new (size_t size, WL::MemLabelRef label, int align, const char* areaName, const char* objectName, const char* file, int line);
extern void* operator new (size_t size, WL::MemLabelRef label, int align, const char* file, int line);
extern void operator delete (void* p, WL::MemLabelRef label, int align, const char* areaName, const char* objectName, const char* file, int line);
extern void operator delete (void* p, WL::MemLabelRef label, int align, const char* file, int line);


INLINE int alignUp(int value, int alignment)
{
	UINT32 ptr = value;
	UINT32 bitMask = (alignment - 1);
	UINT32 lowBits = ptr & bitMask;
	UINT32 adjust = ((alignment - lowBits) & bitMask);
	return adjust;
}

//extern void* operator new (size_t size)noexcept;
//extern void* operator new[](size_t size)noexcept;
//extern void operator delete (void* p)noexcept;
//extern void operator delete[](void* p)noexcept;

extern void* getPreallocatedMemory(int size);
extern void* malloc_internal(size_t size, size_t align, WL::MemLabelRef label, int allocateOptions, const char* file, int line);
extern void* calloc_internal(size_t count, size_t size, int align, WL::MemLabelRef label, int allocateOptions, const char* file, int line);
extern void* realloc_internal(void* ptr, size_t size, int align, WL::MemLabelRef label, int allocateOptions, const char* file, int line);
extern void free_internal(void* ptr);
extern void  free_alloc_internal(void* ptr, WL::MemLabelRef label);

#define WL_MALLOC(label, size)  malloc_internal(size, kDefaultMemoryAlignment, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_MALLOC_NULL(label, size)                 malloc_internal(size, kDefaultMemoryAlignment, label, kAllocateOptionReturnNullIfOutOfMemory, __FILE__, __LINE__)
#define WL_MALLOC_ALIGNED(label, size, align)       malloc_internal(size, align, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_MALLOC_ALIGNED_NULL(label, size, align)  malloc_internal(size, align, label, kAllocateOptionReturnNullIfOutOfMemory, __FILE__, __LINE__)
#define WL_CALLOC(label, count, size)               calloc_internal(count, size, kDefaultMemoryAlignment, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_REALLOC_(label, ptr, size)               realloc_internal(ptr, size, kDefaultMemoryAlignment, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_CALLOC_NULL(label, count, size)          calloc_internal(count, size, kDefaultMemoryAlignment, label, kAllocateOptionReturnNullIfOutOfMemory, __FILE__, __LINE__)
#define WL_REALLOC(label, ptr, size)                realloc_internal(ptr, size, kDefaultMemoryAlignment, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_REALLOC_NULL(label, ptr, size)           realloc_internal(ptr, size, kDefaultMemoryAlignment, label, kAllocateOptionReturnNullIfOutOfMemory, __FILE__, __LINE__)
#define WL_REALLOC_ALIGNED(label, ptr, size, align) realloc_internal(ptr, size, align, label, kAllocateOptionNone, __FILE__, __LINE__)
#define WL_REALLOC_ALIGNED_NULL(label, ptr, size, align) realloc_internal(ptr, size, align, label, kAllocateOptionReturnNullIfOutOfMemory, __FILE__, __LINE__)
#define WL_FREE(label, ptr)                         free_alloc_internal(ptr, label)


template<typename T>
INLINE void delete_internal(T* ptr, WL::MemLabelRef label)
{
	if (nullptr != ptr)
	{
		ptr->~T();
	}
	WL_FREE(label, (void*)ptr);
}

#define HEAP_NEW(cls) new (getPreallocatedMemory(sizeof(cls))) cls
#define HEAP_DELETE(obj, cls) obj->~cls(); obj = nullptr;

#define WL_NEW(type, label) new (WL::MemLabelRef(kMem##label##Id), WL::kDefaultMemoryAlignment, __FILE__, __LINE__) type
#define WL_DELETE(ptr, label) { delete_internal(ptr, WL::MemLabelRef(kMem##label##Id)); ptr = nullptr; }

//#define WL_NEW_ARRAY(type, count, label) new (label, WL::kDefaultMemoryAlignment, __FILE__, __LINE__) type
#define WL_NEW_ARRAY(type, count) new type[count]
#define WL_DELETE_ARRAY(ptr) { if(ptr) delete[](ptr);}

#define WL_NEW_ALIGNED(type, label, align) new (label, align, __FILE__, __LINE__) type


