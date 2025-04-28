#pragma once
#define ErrorString(x)		//				do{ DebugStringToFile (x, 0, __FILE_STRIPPED__, __LINE__, kError); }while(0)
#define Assert(x)				{  }
#define AssertString(x)			{  }
#define AssertMsg(x,msg)		{  }
#define AssertFormatMsg(x,...)	{  }
#define DebugAssert(x)					Assert(x)
#define DebugAssertMsg(x, msg)			AssertMsg(x, msg)
#define DebugAssertFormatMsg(x, ...)	AssertFormatMsg(x, __VA_ARGS__)
#define FatalErrorMsg(...)			

namespace WL
{
#ifdef __cplusplus
	extern "C"
	{
		extern void printf_console(const char* log, ...);
	}
#endif

}