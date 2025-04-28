#pragma once
#include "RendererPass.h"

namespace WL
{
	class CLightMeshRendererPass : public CRendererPass
	{
		typedef CRendererPass Parent;
		friend class CDeferredRendererPass;
	public:
		CLightMeshRendererPass(CRenderer* pRenderer);
		virtual ~CLightMeshRendererPass();
		virtual bool initialise() final;

	private:
		virtual bool initlaliseMaterial()final;
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEntity(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual void drawEnd(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)final;
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)final;
		virtual void commitToGpu(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderUnitGroup)final;
		virtual void clearSurface()final;
	};
}