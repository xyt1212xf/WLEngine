#pragma once
#include "RendererPass.h"

namespace WL
{
	class CDeferredLightRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
		friend class CDeferredRendererPass;
	public:
		CDeferredLightRendererPass(CRenderer* pRenderer);
		virtual ~CDeferredLightRendererPass();
		virtual bool initialise() final;

	private:
		virtual bool initlaliseMaterial()final;
		virtual void initlaliseOutputRT()final;
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual void drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)final;
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)final;

	private:
		CMaterialInstance* mpStencilIns = nullptr;
		CActorEntity* mpFullScreenActor = nullptr;
		SMatrix44	mFullScreenMT;
		SMatrix44	mFullScreenTransposeMT;
	};
}