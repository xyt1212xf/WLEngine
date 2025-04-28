#pragma once
#include "Common.h"
namespace WL
{
	struct Matrix44
	{
		static const Matrix44& identity() 
		{
			static  Matrix44 m(	1.0f, 0, 0, 0,
								0, 1.0f, 0, 0,
								0, 0, 1.0f, 0,
								0, 0, 0, 1.0f);
			return m;
		}

		Matrix44() = default;
		
		explicit Matrix44(const float* fStream);
		Matrix44(const Matrix44& m);
		Matrix44(Matrix44&& m);
		Matrix44(float f11, float f12, float f13, float f14,
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

		void operator = (const Matrix44& mat);
		void operator = (Matrix44&& mat);

		bool operator == (const Matrix44& mat) const;
		bool operator != (const Matrix44& mat) const;

		Matrix44 operator * (const Matrix44& mat) const;
		Matrix44 operator + (const Matrix44& mat) const;
		Matrix44 operator - (const Matrix44& mat) const;
		Matrix44 operator / (float f) const;
		Matrix44 operator * (float f) const;

		void operator *= (const Matrix44& mat);
		void operator += (const Matrix44& mat);
		void operator -= (const Matrix44& mat);
		void operator /= (float f);
		void operator *= (float f);

		void* data();
		friend Matrix44 operator * (float f, const Matrix44& mat);

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

	struct Matrix43
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