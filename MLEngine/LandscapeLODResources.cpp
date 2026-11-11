// LandscapeLODResources.cpp
#include "LandscapeLODResources.h"
#include "LandscapeVertexFactory.h"
#include "LandscapeHeightfield.h"
#include "RHIbuffer.h"
#include "GraphicPlug.h"

namespace ML
{
	namespace
	{
		float HF_HeightClamped(const FLandscapeHeightfield& HF, int32 x, int32 y)
		{
			x = (x < 0) ? 0 : (x > HF.ComponentSizeQuads ? HF.ComponentSizeQuads : x);
			y = (y < 0) ? 0 : (y > HF.ComponentSizeQuads ? HF.ComponentSizeQuads : y);
			return HF.GetHeight(x, y) * HF.ZScale;
		}
	}

	CLandscapeLODResources::~CLandscapeLODResources()
	{

	}

	void CLandscapeLODResources::Build(const FLandscapeHeightfield& HF, int32 LODIndex, CLandscapeVertexFactory* InVF, uint32 NumLayers /*= 1*/)
	{
		Release();
		VF = InVF;
		const int32 Step = 1 << LODIndex;
		const int32 Quads = HF.ComponentSizeQuads / Step;
		const int32 N = Quads + 1;
		NumVertices = N * N;

		std::vector<Vec3F> Pos;
		Pos.reserve(NumVertices);

		std::vector<Vec3F> Nor;
		Nor.reserve(NumVertices);

		std::vector<Vec2F> UV;
		UV.reserve(NumVertices);

		// Landscape layer blend (UE: weightmap per component). P0 carries the
		// blend weights in the COLOR vertex stream instead of a weightmap texture
		// - one weight per vertex per layer, slice i = COLOR channel i:
		//   r = slice 0 base (grass), g = slice 1 rock (steep slopes),
		//   b = slice 2 snow (peaks), a = slice 3 dirt (valleys).
		// Layers beyond NumLayers keep zero weight, so a 2-layer material still
		// blends correctly. Upgrade path (UE parity): sculpt/paint writes real
		// weightmaps; the shader interface stays unchanged.
		const uint32 LayerCount = (NumLayers > 0 && NumLayers <= 4) ? NumLayers : 1u;

		// Thresholds are tuned for the current placeholder heightfield
		// (h = 0.5*(sin + cos), raw heights in [-1, 1], ZScale = 1). They move
		// into authored weightmaps once painting exists - do not treat them as
		// final tuning for arbitrary terrain.
		auto Smooth = [](float A, float B, float X)
			{
				float t = (X - A) / (B - A);
				t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
				return t * t * (3.0f - 2.0f * t);
			};
		// Slope band (n-space): the placeholder terrain's normalized n.y spans
		// ~[0.99, 1.0], i.e. slope = 1 - n.y in ~[0, 0.01].
		constexpr float RockSlopeStart = 0.004f;
		constexpr float RockSlopeEnd   = 0.010f;
		// Height bands in raw height units ([-1, 1] placeholder range).
		constexpr float SnowHeightStart = 0.40f;
		constexpr float SnowHeightEnd   = 0.70f;
		constexpr float DirtHeightHigh  = -0.40f;
		constexpr float DirtHeightLow   = -0.70f;

		std::vector<Vec4F> Col;
		Col.reserve(NumVertices);

		for (int32 y = 0; y < N; ++y)
		{
			for (int32 x = 0; x < N; ++x)
			{
				const int32 sx = x * Step, sy = y * Step;
				Pos.push_back(Vec3F((float)sx, HF_HeightClamped(HF, sx, sy), (float)sy));
				UV.push_back(Vec2F((float)x / (float)Quads, (float)y / (float)Quads));
				const float hL = HF_HeightClamped(HF, sx - 1, sy);
				const float hR = HF_HeightClamped(HF, sx + 1, sy);
				const float hD = HF_HeightClamped(HF, sx, sy - 1);
				const float hU = HF_HeightClamped(HF, sx, sy + 1);
				Vec3F n(hL - hR, 2.0f, hD - hU);
				n.normalize();
				Nor.push_back(n);

				// Per-vertex layer weights, painted sequentially onto the grass
				// base so the total always sums to 1 (shader re-normalizes anyway).
				const float Slope = 1.0f - n.y;                       // 0 = flat
				const float Height = HF.GetHeight(sx, sy);            // raw height
				float Grass = 1.0f, Rock = 0.0f, Snow = 0.0f, Dirt = 0.0f;
				if (LayerCount >= 2)
				{
					Rock = Smooth(RockSlopeStart, RockSlopeEnd, Slope);
					Grass -= Rock;
				}
				if (LayerCount >= 3)
				{
					Snow = Smooth(SnowHeightStart, SnowHeightEnd, Height) * Grass;
					Grass -= Snow;
				}
				if (LayerCount >= 4)
				{
					Dirt = (1.0f - Smooth(DirtHeightLow, DirtHeightHigh, Height)) * Grass;
					Grass -= Dirt;
				}
				Col.push_back(Vec4F(Grass, Rock, Snow, Dirt));
			}
		}

		std::vector<uint32> Idx;
		for (int32 y = 0; y < Quads; ++y)
		{
			for (int32 x = 0; x < Quads; ++x)
			{
				const uint32 a = y * N + x, b = a + 1, c = a + N, d = c + 1;
				Idx.push_back(a);   // 翻转绕序：使顶面成为正面(CCW)，
				Idx.push_back(b);   // 与 cube 在 FrontCounterClockwise=TRUE + CullMode=BACK 下一致
				Idx.push_back(c);
				Idx.push_back(b);
				Idx.push_back(d);
				Idx.push_back(c);
			}
		}
		NumIndices = (uint32)Idx.size();

		VertexBuffers.Position = new CRHIBuffer();
		VertexBuffers.Position->Create(EBufferType::Vertex, (UINT32)Pos.size() * sizeof(Vec3F), sizeof(Vec3F), Pos.data());

		VertexBuffers.Normal = new CRHIBuffer();
		VertexBuffers.Normal->Create(EBufferType::Vertex, (UINT32)Nor.size() * sizeof(Vec3F), sizeof(Vec3F), Nor.data());

		VertexBuffers.UV = new CRHIBuffer();
		VertexBuffers.UV->Create(EBufferType::Vertex, (UINT32)UV.size() * sizeof(Vec2F), sizeof(Vec2F), UV.data());

		VertexBuffers.Color = new CRHIBuffer();
		VertexBuffers.Color->Create(EBufferType::Vertex, (UINT32)Col.size() * sizeof(Vec4F), sizeof(Vec4F), Col.data());

		IndexBuffer = new CRHIBuffer();
		IndexBuffer->Create(EBufferType::Index, (UINT32)Idx.size() * sizeof(uint32), sizeof(uint32), Idx.data());

		VF->NumVertices = NumVertices;
		uint32 slot = 0;
		auto Fill = [&](FDataStreamUnit& u, ID3D12Resource* buf, uint32 stride, DXGI_FORMAT fmt)
			{
				u.GpuBuffer = buf;
				u.Slot = slot++;
				u.Offset = 0;
				u.Stride = stride;
				u.Format = fmt;
				u.bValid = !!buf;
			};
		Fill(VF->Schema.Position, VertexBuffers.Position->GetResource(), sizeof(Vec3F), DXGI_FORMAT_R32G32B32_FLOAT);
		Fill(VF->Schema.Normal, VertexBuffers.Normal->GetResource(), sizeof(Vec3F), DXGI_FORMAT_R32G32B32_FLOAT);
		Fill(VF->Schema.UV0, VertexBuffers.UV->GetResource(), sizeof(Vec2F), DXGI_FORMAT_R32G32_FLOAT);
		Fill(VF->Schema.Color, VertexBuffers.Color->GetResource(), sizeof(Vec4F), DXGI_FORMAT_R32G32B32A32_FLOAT);
		VF->BuildInputLayout({ "POSITION", "NORMAL", "TEXCOORD", "COLOR"});
	}

	void CLandscapeLODResources::Release()
	{
		VertexBuffers.Release();
		SafeDelete(IndexBuffer);
		NumVertices = 0; NumIndices = 0;
	}
}
