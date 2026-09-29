#pragma once
#include "Vector.h"
#include "Array.h"

namespace ML
{
	class CMeshAttributes
	{
	protected:
	
		TArray<Vec3F> VertexPositions;
		TArray<Vec3F> VertexNormals;
		TArray<Vec4F> VertexTangents;       // xyz = tangent, w = sign
		TArray<Vec2F> VertexUVs[4];         // ¶àÌ× UV
		TArray<Vec4F> VertexColors;

		TArray<uint32> Indices;
		TArray<uint32> Vertices;

	
		//struct FMaterialSlot
		//{
		//	std::string Name = "";
		//	class CMaterial* Material = nullptr;
		//};
		//TArray<FMaterialSlot> MaterialSlots;
	};
}