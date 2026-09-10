#pragma once
#include "RendererPass.h"

namespace WL
{
	class CShadowMapRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
	public:
		CShadowMapRendererPass(CRenderer* pRenderer);
		virtual ~CShadowMapRendererPass();
		virtual bool initialise() final;

	private:
		virtual bool initlaliseMaterial()final;
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual void drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)final;
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)final;

	private:
		CActorEntity* mpFullScreenActor = nullptr;
		SMatrix44	mFullScreenMT;
		SMatrix44	mFullScreenTransposeMT;
	};
}