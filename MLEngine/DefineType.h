#pragma once

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

