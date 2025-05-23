#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <mutex> 
#include <assert.h>
#include <atomic>
#include <cstddef>
#include <fstream>
#include <cmath>
#include <optional>
#include <forward_list>
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

#ifdef _WINDOWS
#include <windows.h>
#ifdef _MSC_VER
#define NOMINMAX
#endif

#endif 
#include "LogAssert.h"
#include "DefineType.h"
#include "ClassType.h"
#include "Export.h"

#define SafeDeleteArray( pObject ) \
	if(pObject != nullptr)\
	{\
		delete[] pObject;\
		pObject = nullptr;\
	}

#define SafeDelete( pObject ) \
	if(pObject != nullptr)\
	{\
		delete pObject;\
		pObject = nullptr;\
	}

#define WL_DECREASE( pObject ) \
	if(pObject != nullptr)\
	{\
		pObject->release();\
		pObject = nullptr;\
	}
#define DEV_RELEASE( pObject ) \
	if(pObject != nullptr)\
	{\
		pObject->Release();\
		pObject = nullptr;\
	}

#define WL_INCREASE( pObject ) \
	if(pObject != nullptr)\
	{\
		pObject->addRef();\
	}


#define ROUND_UP(size, aligment) (size_t)((size + aligment - 1) & ~(aligment - 1));


namespace WL
{
	enum class CustomKeyWorld
	{
		GlobalLight,
		AmbientLight,
	};

	enum
	{
		INDEX_NONE = -1 
	};

	enum Axis : INT8
	{
		X,
		Y,
		Z
	};

	struct SWindowConfig
	{
		HINSTANCE hInstance = nullptr;
		std::string	szTitle = "";
		int nWidth = 1024;
		int nHeight = 768;
		bool bFullScreen = false;
		bool bVsync = true;
		bool bConsoleWin = true;
		bool bShowFps = true;
		bool bMSAA = false;
		int backBufferFormat = 0;
		int depthBufferFormat = 0;
	};

	enum class commandType
	{
		Invalid = 0,
		ShotScreen = 1,
		StopDraw = 2,
		RecoverDraw = 3,
	};

	struct SCommand
	{
		SCommand(commandType nTyep, void* pData)
		{
			this->nTyep = nTyep;
			this->pData = pData;
		}
		commandType nTyep = commandType::Invalid;
		void* pData = nullptr;
	};
	
	class CNoncopyable
	{
	protected:
		// ensure the class cannot be constructed directly
		CNoncopyable() {}
		// the class should not be used polymorphically
		~CNoncopyable() {}
	private:
		CNoncopyable(const CNoncopyable&);
		CNoncopyable& operator=(const CNoncopyable&);
	};
}
