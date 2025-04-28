#pragma once


typedef char                CHAR;
typedef signed char         INT8;
typedef unsigned char       UCHAR;
typedef unsigned char       UINT8;
typedef unsigned char       BYTE;
typedef short               SHORT;
typedef signed short        INT16;
typedef unsigned short      USHORT;
typedef unsigned short      UINT16;
typedef unsigned short      WORD;
typedef int                 INT;
typedef signed int          INT32;
typedef unsigned int        UINT;
typedef unsigned int        UINT32;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      INT64;
typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;
typedef float           	F32;
typedef double          	F64;
const float		ROUNDING_ERROR_f32 = 0.000001f;
const double	ROUNDING_ERROR_f64 = 0.00000001;
#define InOut 
#define In
#define Out
#define PI_F    ((float)  3.141592654f)
#define PI     3.14159265358979323846
#define TWO_PI 6.283185307179586476925286766559

#define	F32_LOWER_0(n)		((n) <  0.0f)
#define	F32_LOWER_EQUAL_0(n)	((n) <= 0.0f)
#define	F32_GREATER_0(n)	((n) >  0.0f)
#define	F32_GREATER_EQUAL_0(n)	((n) >= 0.0f)
#define	F32_EQUAL_1(n)		((n) == 1.0f)
#define	F32_EQUAL_0(n)		((n) == 0.0f)

#define ROUND_UP(size, aligment) (size_t)((size + aligment - 1) & ~(aligment - 1));
#if defined(__GNUC__) || defined(__SNC__) || defined(__clang__) || defined(__ghs__)
#define ALIGN_OF(T) __alignof__(T)
#define ALIGN_TYPE(val) __attribute__((aligned(val)))
#define INLINE inline __attribute__ ((always_inline))
#elif defined(_MSC_VER)
#define ALIGN_OF(T) __alignof(T)
#define ALIGN_TYPE(val) __declspec(align(val))
#define INLINE __forceinline
#elif defined(__ARMCC_VERSION)
#define ALIGN_OF(T) __alignof__(T)
#define ALIGN_TYPE(val) __attribute__((aligned(val)))  // ARMCC supports GNU extension
#define INLINE __forceinline
#else
#define ALIGN_TYPE(size)
#define INLINE inline
#endif

