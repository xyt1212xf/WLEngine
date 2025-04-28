#pragma once
#include "RendererPass.h"
/*gui三角形的点顺序，顺时针填充
0   1
----|
| / |
|/--|
2   3
*/

namespace WL
{
	class CGuiRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
	public:
		CGuiRendererPass(CRenderer* pRenderer);
		virtual ~CGuiRendererPass();
		virtual bool initialise() final;
		const SamplerState* getSamplerState() const;

	public:
		static const std::string szMaterName;
		static const std::string szfontMaterName;

	private:
		virtual bool initlaliseMaterial()final;
		virtual void drawEntity(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount) final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderUnitGroup)final;
		virtual void drawFinal() final;
		CGpuBuffer* mpVBBuffer = nullptr;
		CGpuBuffer* mpIBBuffer = nullptr;
		CMaterialInstance* mpMaterialIns = nullptr;
		CMaterialInstance* mpFontMaterialIns = nullptr;
		std::vector<VertexVTC>	mVertics;
		std::vector<UINT16>		mIndices; 
		constexpr int getVertexSize() const;
		int mDataLength = 0;
		int mVertexSize = sizeof(VertexVTC);
		Matrix44 mWorldTransposeMT;
		Matrix44 mViewTransposeMT;
		Matrix44 mOrthogTransposeMT;
		static int nMaxCount;


	};
}