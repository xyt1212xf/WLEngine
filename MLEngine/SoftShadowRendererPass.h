#pragma once
#include "RendererPass.h"
namespace WL
{
	class CSoftShadowRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
	public:
		CSoftShadowRendererPass(CRenderer* pRenderer);
		virtual ~CSoftShadowRendererPass();

	private:
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState)final;
		virtual void drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual void drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)final;

		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)final;
		virtual bool initlaliseMaterial()final;
		virtual void initlaliseOutputRT()final;
	};
}