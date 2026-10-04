#pragma once
#include "Common.h"
#include "Vector.h"
#include "Quaternion.h"

#pragma warning(push)
#pragma warning(disable: 4201)

namespace ML
{
	struct FMatrix44
	{
		static const FMatrix44& identity() 
		{
			static  FMatrix44 m(	1.0f, 0, 0, 0,
								0, 1.0f, 0, 0,
								0, 0, 1.0f, 0,
								0, 0, 0, 1.0f);
			return m;
		}

		FMatrix44() = default;
		
		explicit FMatrix44(const float* fStream);
		FMatrix44(const FMatrix44& m);
		FMatrix44(FMatrix44&& m);
		FMatrix44(float f11, float f12, float f13, float f14,
				 float f21, float f22, float f23, float f24,
				 float f31, float f32, float f33, float f34,
				 float f41, float f42, float f43, float f44);

		//! Simple operator for directly accessing every element of the matrix.
		float& operator()(const INT32 row, const INT32 col)
		{
			return m[row][col];
		}

		static FMatrix44 MakeRotation(const CQuaternion& Quat)
		{
			// 四元数 → 旋转矩阵（行主序）
			float x = Quat.mx;
			float y = Quat.my;
			float z = Quat.mz;
			float w = Quat.mw;

			float x2 = x * x;
			float y2 = y * y;
			float z2 = z * z;

			FMatrix44 M;
			M.zero();

			M._11 = 1.0f - 2.0f * (y2 + z2);
			M._12 = 2.0f * (x * y + w * z);
			M._13 = 2.0f * (x * z - w * y);
			M._14 = 0.0f;

			M._21 = 2.0f * (x * y - w * z);
			M._22 = 1.0f - 2.0f * (x2 + z2);
			M._23 = 2.0f * (y * z + w * x);
			M._24 = 0.0f;

			M._31 = 2.0f * (x * z + w * y);
			M._32 = 2.0f * (y * z - w * x);
			M._33 = 1.0f - 2.0f * (x2 + y2);
			M._34 = 0.0f;

			M._41 = 0.0f;
			M._42 = 0.0f;
			M._43 = 0.0f;
			M._44 = 1.0f;

			return M;
		}

		static FMatrix44 MakeTranslation(Vec3F pos)
		{
			return MakeTranslation(pos.x, pos.y, pos.z);
		}
		static FMatrix44 MakeTranslation(float x, float y, float z)
		{
			FMatrix44 matrix;
			matrix.zero();
			matrix.m[3][0] = x;
			matrix.m[3][1] = y;
			matrix.m[3][2] = z;
			matrix.m[3][3] = 1;
			return matrix;
		}

		static FMatrix44 MakeScale(Vec3F pos)
		{
			return MakeScale(pos.x, pos.y, pos.z);
		}
		static FMatrix44 MakeScale(float x, float y, float z)
		{
			FMatrix44 matrix;
			matrix.zero();
			matrix.m[0][0] = x;
			matrix.m[1][1] = y;
			matrix.m[2][2] = z;
			return matrix;
		}

		
		//! Simple operator for directly accessing every element of the matrix.
		const float& operator()(const INT32 row, const INT32 col) const 
		{
			return m[row][col]; 
		}

		void zero();

		void transpose();

		void operator = (const FMatrix44& mat);
		void operator = (FMatrix44&& mat);

		bool operator == (const FMatrix44& mat) const;
		bool operator != (const FMatrix44& mat) const;

		FMatrix44 operator * (const FMatrix44& mat) const;
		FMatrix44 operator + (const FMatrix44& mat) const;
		FMatrix44 operator - (const FMatrix44& mat) const;
		FMatrix44 operator / (float f) const;
		FMatrix44 operator * (float f) const;

		void operator *= (const FMatrix44& mat);
		void operator += (const FMatrix44& mat);
		void operator -= (const FMatrix44& mat);
		void operator /= (float f);
		void operator *= (float f);

		void* data();
		friend FMatrix44 operator * (float f, const FMatrix44& mat);

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
	static_assert(sizeof(FMatrix44) == sizeof(float) * 16, "Size mismatch");
	static_assert(sizeof(SMatrix43) == sizeof(float) * 12, "Size mismatch");
}

#pragma warning(pop)