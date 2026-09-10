#pragma once
#include "Renderer.h"
#include "ActorEntity.h"

namespace WL
{
	class CFrontRenderer : public CRenderer
	{
		typedef CRenderer Parent;
	public:
		CFrontRenderer();
		virtual ~CFrontRenderer();
	
	private:
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray) final;
		virtual void begin() final;
		virtual void end() final;
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext)final;
	private:
		CActorEntity* mpScreen = nullptr;
		CSurfaceView* mpRenderSurface;
		CRendererPass* mpRenderPass = nullptr;
		//RenderTargetView* mpRenderTargetView = nullptr;
		//DepthStencilView* mpDepthStencilView = nullptr;
	};
}