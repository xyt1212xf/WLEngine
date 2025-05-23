#pragma once
#include "Common.h"
namespace WL
{
	struct SMatrix44
	{
		static const SMatrix44& identity() 
		{
			static  SMatrix44 m(	1.0f, 0, 0, 0,
								0, 1.0f, 0, 0,
								0, 0, 1.0f, 0,
								0, 0, 0, 1.0f);
			return m;
		}

		SMatrix44() = default;
		
		explicit SMatrix44(const float* fStream);
		SMatrix44(const SMatrix44& m);
		SMatrix44(SMatrix44&& m);
		SMatrix44(float f11, float f12, float f13, float f14,
				 float f21, float f22, float f23, float f24,
				 float f31, float f32, float f33, float f34,
				 float f41, float f42, float f43, float f44);

		//! Simple operator for directly accessing every element of the matrix.
		float& operator()(const INT32 row, const INT32 col)
		{
			return m[row][col];
		}

		//! Simple operator for directly accessing every element of the matrix.
		const float& operator()(const INT32 row, const INT32 col) const 
		{
			return m[row][col]; 
		}

		void zero();

		void transpose();

		void operator = (const SMatrix44& mat);
		void operator = (SMatrix44&& mat);

		bool operator == (const SMatrix44& mat) const;
		bool operator != (const SMatrix44& mat) const;

		SMatrix44 operator * (const SMatrix44& mat) const;
		SMatrix44 operator + (const SMatrix44& mat) const;
		SMatrix44 operator - (const SMatrix44& mat) const;
		SMatrix44 operator / (float f) const;
		SMatrix44 operator * (float f) const;

		void operator *= (const SMatrix44& mat);
		void operator += (const SMatrix44& mat);
		void operator -= (const SMatrix44& mat);
		void operator /= (float f);
		void operator *= (float f);

		void* data();
		friend SMatrix44 operator * (float f, const SMatrix44& mat);

		//////////////////////////////////////////////////////////////////////////
		//data
		union 
		{
			struct 
			{
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};
	};

	struct SMatrix43
	{

		//////////////////////////////////////////////////////////////////////////
		//data
		union
		{
			struct 
			{
				float        _11, _12, _13;
				float        _21, _22, _23;
				float        _31, _32, _33;
				float        _41, _42, _43;

			};
			float m[4][3];
		};
	};

}