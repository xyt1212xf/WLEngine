#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <tuple>
#include <list>
#include "Common.h"
#include "MathLib.h"
#include "Vector.h"
#include "ModelInstance.h"
#include "lua.hpp"
#ifndef V_RETURN
#define V_RETURN(x)					{ hr = x; if( FAILED(hr) ) { return hr; } }
#endif

template<typename T, size_t n>
char(&arraySize(const T(&)[n]))[n];
#define CountArray( array ) (sizeof(arraySize( array ) ))
#define PAD16(n) (((n)+15)/16*16)

namespace WL
{
	enum GeomeTryType : UINT8
	{
		//立方体
		CUBOID,
		//楔型、直三棱柱
		WEDGE,
		//直四棱锥
		PYRAMID,
		//圆柱
		CYLINDER,
		//圆锥
		CONE,
		//球体
		SPHERE,
		//镂空组合体
		COMPOSITE,
	};
	namespace Foundation
	{
		// is value a power-of-two
		template<typename T>
		INLINE bool isPowerOfTwo(T mask)
		{
			return (mask & (mask - 1)) == 0;
		}


		const signed char gHighestBitLut[] = { -1,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3 };
		INLINE int highestBit(UINT32 mask)
		{
#if defined(SN_TARGET_PS3) || defined(CAFE)
			return 31 - __cntlzw(mask);
#else
			int base = 0;

			if (mask & 0xffff0000)
			{
				base = 16;
				mask >>= 16;
			}
			if (mask & 0x0000ff00)
			{
				base += 8;
				mask >>= 8;
			}
			if (mask & 0x000000f0)
			{
				base += 4;
				mask >>= 4;
			}

			return base + gHighestBitLut[mask];
#endif
		}

		INLINE DWORD FtoDw(float f)
		{
			return *((DWORD*)&f);
		}

		INLINE float getRandomFloat(float lowBound, float highBound)
		{
			if (lowBound >= highBound) // bad input
				return lowBound;

			// get random float in [0, 1] interval
			float f = (rand() % 10000) * 0.0001f;

			// return float in [lowBound, highBound] interval. 
			return (f * (highBound - lowBound)) + lowBound;
		}

		INLINE void getRandomVector(Vec3F* out, Vec3F* min, Vec3F* max)
		{
			out->x = getRandomFloat(min->x, max->x);
			out->y = getRandomFloat(min->y, max->y);
			out->z = getRandomFloat(min->z, max->z);
		}

		//! returns if a equals b, taking possible rounding errors into account
		INLINE bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_f32)
		{
			return (a + tolerance >= b) && (a - tolerance <= b);
		}

		//! returns if a equals b, taking possible rounding errors into account
		INLINE bool equals(const double a, const double b, const double tolerance = ROUNDING_ERROR_f64)
		{
			return (a + tolerance >= b) && (a - tolerance <= b);
		}

		template<class T>
		INLINE T Min(const T& a, const T& b)
		{
			return ((a) < (b) ? (a) : (b));
		}

		template<class T>
		INLINE T Max(const T& a, const T& b)
		{
			return ((a) > (b) ? (a) : (b));
		}

		INLINE bool IsSSE()
		{
#ifdef WIN64
			return false;
#else
			const DWORD _SSE_FEATURE_BIT = 0x02000000; 
			DWORD dwFeature = 0;
			try
			{
				_asm
				{
					mov eax, 1
					cpuid
					mov dwFeature, edx
				}
				if (dwFeature & _SSE_FEATURE_BIT)
				{
					__asm
					{
						xorps xmm0, xmm0  
					}
				}
			}
			catch (...)
			{
				return false;
			}
			return true;
#endif
		}

		INLINE bool IsSSE2()
		{
#ifdef WIN64
			return false;
#else
			const DWORD _SSE2_FEATURE_BIT = 0x04000000; 
			
			DWORD dwFeature = 0;
			try
			{
				_asm
				{
					mov eax, 1
					cpuid
					mov dwFeature, edx
				}
				if (dwFeature & _SSE2_FEATURE_BIT)
				{
					__asm
					{
						xorpd xmm0, xmm0
					}
				}
			}
			catch (...)
			{
				return false;
			}
			return true;
#endif
		}
		INLINE INT32 calculateTableLength(lua_State* pLua)
		{
			INT32 nCount = 1;
			INT32 _t = lua_gettop(pLua);
			lua_pushnil(pLua);

			while (lua_next(pLua, -2))
			{
				lua_type(pLua, -2);
				lua_pop(pLua, 1);
				++nCount;
			}
			lua_settop(pLua, _t);
			return nCount;
		}

		INLINE void OutputDebugPrintf(const char* strOutputString, ...)
		{
			char strBuffer[4096] = { 0 };
			va_list vlArgs;
			va_start(vlArgs, strOutputString);
			_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
			//vsprintf(strBuffer,strOutputString,vlArgs);
			va_end(vlArgs);
			OutputDebugStringA(strBuffer);
		}

		INLINE std::list<std::string> split(std::string str, std::string pattern)
		{
			std::list<std::string> result;
			if (pattern.length() < str.length())
			{
				std::string::size_type pos;

				str += pattern;
				size_t size = str.size();

				for (size_t i = 0; i < size; i++)
				{
					pos = str.find(pattern, i);
					if (pos != std::string::npos)
					{
						std::string s = str.substr(i, pos - i);
						result.push_back(s);
						i = pos + pattern.size() - 1;
					}
				}
			}
			return result;
		}


		INLINE bool strtowstr(IN const std::string& str, OUT std::wstring& wstr)
		{
			wstr.clear();
			wstr.resize(str.length());
			WCHAR* pBuf = const_cast<WCHAR*>(wstr.c_str());
			INT32 nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (INT32)str.length(), NULL, 0);
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), (INT32)str.length(), pBuf, nwLen);
			return true;
		}

		INLINE bool wstrtostr(IN const std::wstring& wstr, OUT std::string& str)
		{
			int iSize;
			char pszMultiByte[512];
			iSize = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszMultiByte, iSize, NULL, NULL);
			str = pszMultiByte;
			return true;
		}

		template <typename T>
		INLINE void Clamp(OUT T& out, IN T p1, IN T p2)
		{
			if (out < p1)
			{
				out = p1;
			}
			else if (out > p2)
			{
				out = p2;
			}
		}


		template<class T >
		T Clip(T minvalue, T maxvalue, T in)
		{
			if (in >= minvalue && in <= maxvalue)
			{
				return in;
			}
			else if (in < minvalue)
			{
				return minvalue;
			}
			return maxvalue;
		}

		INLINE void getPath(TCHAR* path, TCHAR* szbuffer)
		{
			for (size_t i = _tcslen(szbuffer); i > 0; --i)
			{
				if ('/' == szbuffer[i])
				{
					_tcsncpy_s(path, i + 1, szbuffer, i + 1);
					path[i + 1] = '\0';
					break;
				}
			}
		}

		INLINE bool InPlace(RECT const& _rect, POINT const& _pt)
		{
			return (
				_pt.x >= _rect.left &&
				_pt.x < _rect.left + _rect.right &&			
				_pt.y >= _rect.top &&
				_pt.y < _rect.top + _rect.bottom			
				);
		}

		template<typename T>
		class IsClassT
		{
		private:
			typedef char One;
			typedef struct
			{
				char a[2];
			} Two;
			template<typename C>
			static One test(int T::*);
			template<typename C>
			static Two test(...);
		public:
			enum { Yes = sizeof(test<T>(0)) == 1 };
			enum { No = !Yes };
		};

		template< typename T >
		bool CheckClass()
		{
			if (IsClassT<T>::Yes)
			{
				return true;
			}
			return false;
		};

		template<typename T>
		T stringToType(const std::string& strConent)
		{
			std::istringstream iss(strConent);
			T num;
			iss >> num;
			return num;
		}

		template<typename T>
		std::vector<T> splitString(std::string str, const std::string& pattern) //noexcept
		{
			std::vector<T> result;
			if (str.length() < 1)
			{
				return result;
			}
			result.reserve(4);

			std::string::size_type pos = 0;
			str += pattern;
			std::string::size_type size = str.size();
			for (std::string::size_type i = 0; i < size; ++i)
			{
				pos = str.find(pattern, i);
				if (pos < size)
				{
					T tempValue = stringToType<T>(str.substr(i, pos - i));
					result.push_back(tempValue);
					i = pos + pattern.size() - 1;
				}
			}
			return result;
		}
		extern std::string readJsonFile(const std::string& szName);
		extern Vec2I changeToScreenCoordinate(const Vec3F& vPos);
		extern std::string getFileExtension(const std::string& name);
		extern std::string getFile(const std::string& name);
		extern std::string getFilePath(const std::string& name);

		extern std::tuple< std::vector<std::string>, std::vector<std::string> >
			getFileInfoList(std::string&& stringDir);

		extern const std::string getExterName(const std::string& strName);

		extern CModelInstance* generateFanGeometry(const Vec3F& vOrigin,
			const Vec3F& fBeginPos,
			const Vec3F& fEndPos,
			const std::string& szMaterial,
			const std::string& szTexture);

		extern CModelInstance* generateModel(void* pVertices,
			int nVertexCount,
			int nVertexFormatSize,
			void* pIndices,
			int nIndexCount,
			int nIndexFormatSize,
			const std::string& szMaterial,
			const std::string& szTexture,
			const std::string& szTextureType = "baseTexture");

		extern CModelInstance* generateBaseGeometryMesh(GeomeTryType type,	
			const std::string& szMaterial,
			const std::string& szTexture,
			const std::string& szTextureType = "baseTexture");

		template<typename T1, typename T2>
		bool toClass(T2 object)
		{
			return dynamic_cast<T1>(object) != nullptr ? true : false;
		}

		template<class T>
		constexpr const T& clamp(const T& v, const T& lo, const T& hi)
		{
			return (v < lo) ? lo : (hi < v) ? hi : v;
		}
	
	}
}


