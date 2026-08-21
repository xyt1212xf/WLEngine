#pragma once

typedef char                CHAR;
typedef signed char         INT8;
typedef signed char         int8;

typedef unsigned char       UCHAR;
typedef unsigned char       UINT8;
typedef unsigned char       uint8;

typedef unsigned char       BYTE;
typedef short               SHORT;

typedef signed short        INT16;
typedef signed short        int16;

typedef unsigned short      USHORT;
typedef unsigned short      WORD;

typedef unsigned short      UINT16;
typedef unsigned short      uint16;


typedef int                 INT;
typedef signed int          INT32;
typedef signed int          int32;

typedef unsigned int        UINT;
typedef unsigned int        UINT32;
typedef unsigned int        uint32;

typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      INT64;
typedef signed __int64      int64;

typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;
typedef unsigned __int64    uint64;

typedef float           	F32;
typedef double          	F64;
typedef unsigned __int64    SIZE_T;

#define MIN_uint8		((uint8)	0x00)
#define	MIN_uint16		((uint16)	0x0000)
#define	MIN_uint32		((uint32)	0x00000000)
#define MIN_uint64		((uint64)	0x0000000000000000)
#define MIN_int8		((int8)		-128)
#define MIN_int16		((int16)	-32768)
#define MIN_int32		((int32)	0x80000000)
#define MIN_int64		((int64)	0x8000000000000000)

#define MAX_uint8		((uint8)	0xff)
#define MAX_uint16		((uint16)	0xffff)
#define MAX_uint32		((uint32)	0xffffffff)
#define MAX_uint64		((uint64)	0xffffffffffffffff)
#define MAX_int8		((int8)		0x7f)
#define MAX_int16		((int16)	0x7fff)
#define MAX_int32		((int32)	0x7fffffff)
#define MAX_int64		((int64)	0x7fffffffffffffff)

#define MIN_flt			(1.175494351e-38F)			/* min positive value */
#define MAX_flt			(3.402823466e+38F)
#define MIN_dbl			(2.2250738585072014e-308)	/* min positive value */
#define MAX_dbl			(1.7976931348623158e+308)	
namespace WL
{

	class CDeviceBase
	{
	public:
		CDeviceBase() = default;
		virtual ~CDeviceBase(){};

	};

}