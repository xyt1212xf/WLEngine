#pragma once
#include "Common.h"

namespace ML
{
	struct FMeshPolygonGroup
	{
		// 可读名（编辑器中显示），比如 "Body" / "Wheel_0"。
		std::string Name = "";
		INT32 MaterialIndex = INDEX_NONE;

		bool HasMaterial() const { return MaterialIndex != INDEX_NONE; }
	};
}