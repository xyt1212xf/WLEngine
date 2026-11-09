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

	void CLandscapeLODResources::Build(const FLandscapeHeightfield& HF, int32 LODIndex, CLandscapeVertexFactory* InVF)
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

		std::vector<Vec4F> Col;
		Col.resize(NumVertices, Vec4F(1.0f, 1.0f, 1.0f, 1.0f));

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
