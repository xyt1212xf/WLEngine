#pragma once
#include "Common.h"

namespace WL
{
#define EQUALS(a, b) ((a + ROUNDING_ERROR_f32 >= b) && (a - ROUNDING_ERROR_f32 <= b) )
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	struct TVec2
	{
		TVec2()
		{

		}
		TVec2(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		TVec2(std::initializer_list<T> list) 
		{
			auto it = list.begin();
			x = (it != list.end()) ? *it++ : 0;
			y = (it != list.end()) ? *it++ : 0;
		}
		explicit TVec2(const T* array)
		{
			x = array[0];
			y = array[1];
		}

		TVec2(const TVec2<T>& vec)
		{
			x = vec.x;
			y = vec.y;
		}

		TVec2(TVec2<T>&& vec)
		{
			x = vec.x;
			y = vec.y;
		}

		T length() const
		{
			return (T)sqrt((double)x * (double)x + (double)y * (double)y);
		}

		void normalize() 
		{
			T tempValue = length();
#ifdef _DEBUG
			if (0 == tempValue)
			{
				return;
			}
#endif
			tempValue = 1 / tempValue;
			x *= tempValue;
			y *= tempValue;
		}
		bool operator<(const TVec2<T>& other) const
		{
			return x < other.x &&
				y < other.y;
		}

		bool operator == (const TVec2<T>& vec) const
		{
			return	(EQUALS(x, vec.x) && 
					 EQUALS(y, vec.y));
		}

		bool operator != (const TVec2<T>& vec) const
		{
			return	!(EQUALS(x, vec.x) &&
				      EQUALS(y, vec.y));
		}

		TVec2<T>& operator = (const TVec2<T>& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		TVec2<T> operator + (const TVec2<T>& v) const
		{
			return TVec2<T>(x + v.x, y + v.y);
		}

		TVec2<T> operator - (const TVec2<T>& v) const
		{
			return TVec2<T>(x - v.x, y - v.y);
		}

		void operator += (const TVec2<T>& v)
		{
			x += v.x;
			y += v.y;
		}

		void operator -= (const TVec2<T>& v)
		{
			x -= v.x;
			y -= v.y;
		}

		void operator *= (const TVec2<T>& v)
		{
			x *= v.x;
			y *= v.y;
		}

		void operator /= (const TVec2<T>& v)
		{
			x /= v.x;
			y /= v.y;
		}

		friend TVec2<T> operator * (T fValue, TVec2<T>& vec2)
		{
			return TVec2<T>(vec2.x * fValue, vec2.y * fValue);
		}

		TVec2<T> operator * (T fValue) const
		{
			return TVec2<T>(x * fValue, y * fValue);
		}

		TVec2<T> operator / (T fValue) const
		{
			fValue = 1 / fValue;
			return TVec2(x * fValue, y * fValue);
		}

		TVec2<T> operator + (const T fValue) const
		{
			return TVec2<T>(x + fValue, y + fValue);
		}

		TVec2<T> operator - (const T fValue) const
		{
			return TVec2<T>(x - fValue, y - fValue);
		}

		void operator += (const T fValue)
		{
			x += fValue;
			y += fValue;
		}

		void operator -= (const T fValue)
		{
			x -= fValue;
			y -= fValue;
		}

		void operator *= (const T fValue)
		{
			x *= fValue;
			y *= fValue;
		}

		void operator /= (const T fValue)
		{
			x /= fValue;
			y /= fValue;
		}

		TVec2<T> operator - () const
		{
			return TVec2(-x, -y);
		}

		T* data() const
		{
			return &x;
		}
		T* data()
		{
			return &x;
		}
		//////////////////////////////////////////////////////////////////////////
		T x = 0;
		T y = 0;
	};
	using Vec2F = TVec2<float>;
	using Vec2I = TVec2<INT32>;
	using Vec2U32 = TVec2<UINT32>;
	using Vec2U16 = TVec2<UINT16>;
	using Vec2U8 = TVec2<UINT8>;

	//////////////////////////////////////////////////////////////////////////
	template <class T>
	struct TVec3
	{
		TVec3()
		{

		}
		TVec3(T x, T y, T z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		// Initializer list constructor
		TVec3(std::initializer_list<T> list)
		{
			auto it = list.begin();
			x = (it != list.end()) ? *it++ : 0;
			y = (it != list.end()) ? *it++ : 0;
			z = (it != list.end()) ? *it++ : 0;
		}
		
		explicit TVec3(const T* array)
		{
			x = array[0];
			y = array[1];
			z = array[2];
		}

		TVec3(const TVec3<T>& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		TVec3(TVec3<T>&& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		// casting
		operator float* ()
		{
			return (float*)&x;
		}

		operator const float* () const
		{
			return (const float*)&x;
		}

		T length() const
		{
			return (T)sqrt((double)x * (double)x + (double)y * (double)y + (double)z * (double)z);
		}

		double lengthSqrt() const 
		{
			return (double)x * (double)x + (double)y * (double)y + (double)z * (double)z;
		}

		void normalize() 
		{
			T tempValue = length();
#ifdef _DEBUG
			if(0 == tempValue)
			{
				return;
			}
#endif
			tempValue = 1 / tempValue;
			x *= tempValue;
			y *= tempValue;
			z *= tempValue;
		}
		bool operator<(const TVec3<T>& other) const
		{
			return x < other.x &&
				y < other.y &&
				z < other.z;
		}
		bool operator == (const TVec3<T>& vec) const
		{
			return	(EQUALS(x, vec.x) &&
					 EQUALS(y, vec.y) &&
					 EQUALS(z, vec.z));
		}

		bool operator != (const TVec3<T>& vec) const
		{
			return	!(EQUALS(x, vec.x) &&
					  EQUALS(y, vec.y) &&
				      EQUALS(z, vec.z));
		}

		TVec3<T>& operator = (const TVec3<T>& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		TVec3<T>& operator = (float* v)
		{
			x = v[0];
			y = v[1];
			z = v[2];
			return *this;
		}

		TVec3<T> operator + (const TVec3<T>& v) const
		{
			return TVec3(x + v.x, y + v.y, z + v.z);
		}

		TVec3<T> operator - (const TVec3<T>& v) const
		{
			return TVec3(x - v.x, y - v.y, z - v.z);
		}
		
		TVec3<T> operator * (const TVec3<T>& v) const
		{
			return TVec3(x * v.x, y * v.y, z * v.z);
		}

		TVec3<T> operator / (const TVec3<T>& v) const
		{
			return TVec3(x / v.x, y / v.y, z / v.z);
		}

		void operator += (const TVec3<T>& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}


		void operator -= (const TVec3<T>& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		void operator *= (const TVec3<T>& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
		}

		void operator /= (const TVec3<T>& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
		}

		friend TVec3 operator * (T fValue, const TVec3<T>& vec3)
		{
			return TVec3<T>(vec3.x * fValue, vec3.y * fValue, vec3.z * fValue);
		}

		TVec3<T> operator - () const
		{
			return TVec3(-x, -y, -z);
		}

		TVec3<T> operator * (T fValue) const
		{
			return TVec3<T>(x * fValue, y * fValue, z * fValue);
		}

		TVec3<T> operator / (T fValue) const
		{
			fValue = 1 / fValue;
			return TVec3<T>(x * fValue, y * fValue, z * fValue);
		}

		TVec3<T> operator + (T fValue) const
		{
			return TVec3<T>(x + fValue, y + fValue, z + fValue);
		}

		TVec3<T> operator - (T fValue) const
		{
			return TVec3<T>(x - fValue, y - fValue, z - fValue);
		}

		void operator += (T fValue)
		{
			x += fValue;
			y += fValue;
			z += fValue;
		}

		void operator -= (T fValue)
		{
			x -= fValue;
			y -= fValue;
			z -= fValue;
		}

		void operator *= (T fValue)
		{
			x *= fValue;
			y *= fValue;
			z *= fValue;
		}

		void operator /= (T fValue)
		{
			fValue = 1 / fValue;
			x *= fValue;
			y *= fValue;
			z *= fValue;
		}
		T* data()
		{
			return &x;
		}
		T* data() const
		{
			return &x;
		}
		//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
		T x = 0;
		T y = 0;
		T z = 0;
#endif
	};
	using Vec3F = TVec3<float>;
	using Vec3I = TVec3<INT32> ;
	using Vec3U32 = TVec3<UINT32>;
	using Vec3U16 = TVec3<UINT16>;
	using Vec3U8 = TVec3<UINT8>;
	//////////////////////////////////////////////////////////////////////////

	template <class T>
	struct TVec4
	{
		TVec4()
		{

		}
		TVec4(T x, T y, T z, T w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		// Initializer list constructor
		TVec4(std::initializer_list<T> list)
		{
			auto it = list.begin();
			x = (it != list.end()) ? *it++ : 0;
			y = (it != list.end()) ? *it++ : 0;
			z = (it != list.end()) ? *it++ : 0;
			w = (it != list.end()) ? *it++ : 0;
		}

		explicit TVec4(const T* array)
		{
			x = array[0];
			y = array[1];
			z = array[2];
			w = array[3];
		}

		TVec4(const TVec4<T>& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			w = vec.w;
		}

		TVec4(TVec4<T>&& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			w = vec.w;
		}

		T length() const
		{
			return (T)sqrt((double)x * (double)x + (double)y * (double)y + (double)z * (double)z + (double)w * (double)w);
		}

		void normalize()
		{
			T tempValue = length();
#ifdef _DEBUG
			if (0 == tempValue)
			{
				return;
			}
#endif
			tempValue = 1 / tempValue;
			x *= tempValue;
			y *= tempValue;
			z *= tempValue;
			w *= tempValue;
		}
		bool operator<(const TVec4<T>& other) const
		{
			return x < other.x &&
				y < other.y &&
				z < other.z &&
				w < other.w;
		}
		bool operator == (const TVec4<T>& vec) const
		{
			return	(EQUALS(x, vec.x) &&
					 EQUALS(y, vec.y) &&
					 EQUALS(z, vec.z) &&
					 EQUALS(w, vec.w));

		}

		bool operator != (const TVec4<T>& vec) const
		{
			return	!(EQUALS(x, vec.x) &&
					  EQUALS(y, vec.y) &&
					  EQUALS(z, vec.z) &&
					  EQUALS(w, vec.w));
		}

		TVec4<T>& operator = (const TVec4<T>& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}
		TVec4<T> operator - () const
		{
			return TVec4(-x, -y, -z, -w);
		}

		T* data() const
		{
			return &x;
		}
		T* data()
		{
			return &x;
		}
		//////////////////////////////////////////////////////////////////////////
		T x = 0;
		T y = 0;
		T z = 0;
		T w = 0;
	};
	using Vec4F = TVec4<float>;
	using Vec4I = TVec4<INT32>;
	using Vec4U32 = TVec4<UINT32>;
	using Vec4U16 = TVec4<UINT16>;
	using Vec4U8 = TVec4<UINT8>;
}

