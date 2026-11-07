#pragma once
#include "Common.h"
#include "Vector.h"

namespace ML
{
	struct FLandscapeHeightfield
	{
		int32  ComponentSizeQuads = 63;
		float  ZScale = 1.0f;
		Vec3F  Origin = Vec3F(0, 0, 0);
		std::vector<float> Heights;   // row-major, (ComponentSizeQuads+1)^2

		int32 GetNumVertices() const { const int32 N = ComponentSizeQuads + 1; return N * N; }
		float GetHeight(int32 X, int32 Y) const { const int32 N = ComponentSizeQuads + 1; return Heights[Y * N + X]; }
		void  SetHeight(int32 X, int32 Y, float H) { const int32 N = ComponentSizeQuads + 1; Heights[Y * N + X] = H; }
		void  Allocate() { Heights.assign(GetNumVertices(), 0.0f); }
	};
}
