#include "LandscapeProxy.h"
#include "LandscapeHeightfield.h"
#include "LandscapeComponent.h"

namespace ML
{
	CLandscapeProxy::CLandscapeProxy(const std::string& Name)
	: CActor(Name) 
	{

	}

	CLandscapeProxy::CLandscapeProxy(std::string&& Name) noexcept
	: CActor(Name)
	{

	}

	void CLandscapeProxy::CreateLandscape(int32 NumX, int32 NumY, int32 SizeQuads, CMaterial* Mat)
	{
		this->NumX = NumX;   // 记录网格维度，供 GetComponent 计算索引
		this->NumY = NumY;
		for (int32 y = 0; y < NumY; ++y)
		{
			for (int32 x = 0; x < NumX; ++x)
			{
				CLandscapeComponent* C = AddComponent<CLandscapeComponent>();
				// CPrimitiveComponent::bVisible 默认 false（只有关卡里显式写 Visible=true 的
				// 静态网格才会被打开）。地形组件是程序化 AddComponent 出来的，必须显式打开，
				// 否则 CreateSceneProxy 传 IsVisible()=false，GetMeshBatch 直接返回 false → 不画。
				C->SetVisible(true);
				C->SetLandscapeMaterial(Mat);
				FLandscapeHeightfield& HF = C->GetHeightfield();
				HF.ComponentSizeQuads = SizeQuads;
				HF.Allocate();
				for (int32 j = 0; j <= SizeQuads; ++j) for (int32 i = 0; i <= SizeQuads; ++i)
				{
					HF.SetHeight(i, j, 0.5f * (sinf(i * 0.2f) + cosf(j * 0.2f)));
				}
				C->SetLocation(Vec3F((float)(x * SizeQuads), 0.0f, (float)(y * SizeQuads)));
				C->BuildRenderData();
				Tiles.push_back(C);
			}
		}
	}

	CLandscapeComponent* CLandscapeProxy::GetComponent(int32 X, int32 Y) const
	{
		if (X < 0 || Y < 0 || X >= NumX || Y >= NumY)
		{
			return nullptr;
		}
		const int32 idx = Y * NumX + X;          // 之前错写成 Y * Tiles.size() + X
		return (idx < (int32)Tiles.size()) ? Tiles[idx] : nullptr;
	}

	CLandscape::CLandscape(const std::string& Name)
	: CLandscapeProxy(Name)
	{

	}

}
