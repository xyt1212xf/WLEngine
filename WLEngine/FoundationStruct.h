#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <xmmintrin.h>//sse header file(include mmx header file)
#include <emmintrin.h>//sse2 header file(include sse header file)

#define float_zero_critical			0.000001f
#define FBX_MaxCount_ChildBone 10
//ÿ�����Ƶ�����ܼ�������Ӱ�졣
#define FBX_MaxCount_BoneIndexSkinWeightPerControlPoint 4

#include <math.h>
namespace WL 
{
#pragma warning ( push, 3 )  // Temporarily setting warning level 3
	template<typename T>
	struct Rect
	{
		Rect(Rect&& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}
		Rect(T x, T y, T z, T w)
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
	typedef Rect<float> RectF;
	typedef Rect<int> RectI;

#pragma warning (pop) 


	struct VertexFormatVTC
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

	struct VertexFormatVC
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
	struct VertexVT
	{
		Vec3F position;
		Vec2F uv;
	};


	struct VertexFormatVTN
	{
		Vec3F	position;
		Vec2F	uv;
		Vec3F	normal;
	};

	//ֻ���omodģ��
	struct VertexFormatOMOD
	{
		Vec3F	position;	
		Vec3F	omodNormal;
		Vec2F	omodUV;
		float	colorOne;
		float	colorTwo;
	};

	//voxel�Ľṹ
	struct VertexFormatVoxel
	{
		Vec3F	position = { 0, 0, 0 };
		Vec3F	normal = { 0, 0, 0 };
		Vec2F	uv = { 0, 0 };
		Vec3F	alphaIndex = { 0, 0, 0 };
		Vec3F   weight = { 0, 0, 0 };
	};

	struct VertexVTC : public VertexVT
	{
		Vec4F color;
	};

	struct VertexFormatVTNT : public VertexFormatVTN
	{
		Vec3F	tangent;
	};

	struct VertexFormatVTNSkin : public VertexFormatVTN
	{
		Vec4I	BoneIndex;
		Vec3F	skinWeight;
	};

	struct VertexFormatVTNTB : public VertexFormatVTN
	{
		Vec3F	tangent;
		Vec3F	binormal;
	};

	struct VertexFormatVTNTBSkin : public VertexFormatVTNTB
	{
		INT32 	boneIndex[4];
		Vec4F 	bonWeight;
	};

	struct Intersection
	{
		UINT32 dwFace;                 // mesh face that was intersected
		float fBary1, fBary2;         // barycentric coords of intersection
		float fDist;                  // distance from ray origin to intersection
		float u, v;                 // texture coords of intersection
		bool bPick = false;
	};

}
