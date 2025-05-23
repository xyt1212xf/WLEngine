#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <xmmintrin.h>//sse header file(include mmx header file)
#include <emmintrin.h>//sse2 header file(include sse header file)

#define float_zero_critical			0.000001f
#define FBX_MaxCount_ChildBone 10
//每个控制点最多受几根骨骼影响。
#define FBX_MaxCount_BoneIndexSkinWeightPerControlPoint 4

#include <math.h>
namespace WL 
{
#pragma warning ( push, 3 )  // Temporarily setting warning level 3
	template<typename T>
	struct SRect
	{
		SRect(SRect&& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}
		SRect(T x, T y, T z, T w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
		union
		{
			struct
			{
				T left;
				T right;
				T top;
				T bottom;
			};
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
		};
	};
	typedef SRect<float> RectF;
	typedef SRect<int> RectI;

#pragma warning (pop) 


	struct SVertexFormatVTC
	{
#ifdef _WINDOWS
		Vec3F position;
		Vec2F uv;
		Vec4F color;
#else
		union
		{
			struct
			{
				float v[3];
			};
			float x, y, z;
		};
		union
		{
			struct
			{
				float m[4];
			};
			float x, y, z, w;
		};
#endif
	};

	struct SVertexFormatVC
	{
#ifdef _WINDOWS
		Vec3F position;
		Vec4F color;
#else
		union
		{
			struct
			{
				float v[3];
			};
			float x, y, z;
		};
		union
		{
			struct
			{
				float m[4];
			};
			float x, y, z, w;
		};
#endif
	};
	struct SVertexVT
	{
		Vec3F position;
		Vec2F uv;
	};


	struct SVertexFormatVTN
	{
		Vec3F	position;
		Vec2F	uv;
		Vec3F	normal;
	};

	//只针对omod模型
	struct SVertexFormatOMOD
	{
		Vec3F	position;	
		Vec3F	omodNormal;
		Vec2F	omodUV;
		float	colorOne;
		float	colorTwo;
	};

	//voxel的结构
	struct SVertexFormatVoxel
	{
		Vec3F	position = { 0, 0, 0 };
		Vec3F	normal = { 0, 0, 0 };
		Vec2F	uv = { 0, 0 };
		Vec3F	alphaIndex = { 0, 0, 0 };
		Vec3F   weight = { 0, 0, 0 };
	};

	struct SVertexVTC : public SVertexVT
	{
		Vec4F color;
	};

	struct SVertexFormatVTNT : public SVertexFormatVTN
	{
		Vec3F	tangent;
	};

	struct SVertexFormatVTNSkin : public SVertexFormatVTN
	{
		Vec4I	BoneIndex;
		Vec3F	skinWeight;
	};

	struct SVertexFormatVTNTB : public SVertexFormatVTN
	{
		Vec3F	tangent;
		Vec3F	binormal;
	};

	struct SVertexFormatVTNTBSkin : public SVertexFormatVTNTB
	{
		INT32 	boneIndex[4];
		Vec4F 	bonWeight;
	};

	struct SIntersection
	{
		UINT32 dwFace;                 // mesh face that was intersected
		float fBary1, fBary2;         // barycentric coords of intersection
		float fDist;                  // distance from ray origin to intersection
		float u, v;                 // texture coords of intersection
		bool bPick = false;
	};

}
