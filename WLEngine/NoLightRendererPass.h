#pragma once
#include "RendererPass.h"

namespace WL
{
	class CNoLightRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
	public:
		CNoLightRendererPass(CRenderer* pRenderer);
		virtual ~CNoLightRendererPass();
		virtual bool initialise() final;

	private:
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState)final;
		virtual void drawEntity(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual void drawEnd(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)final;
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderUnitGroup)final;
	};
}