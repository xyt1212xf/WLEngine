#pragma once
#include "Renderer.h"
#include "ActorEntity.h"
namespace WL
{
	class CDeferredRenderer : public CRenderer
	{
		typedef CRenderer Parent;
	public:
		CDeferredRenderer();
		virtual ~CDeferredRenderer();
	
	private:
		virtual void drawEntity(SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)final;
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray) final;
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState) final;
		virtual void drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext)final;
		virtual void begin()final;
		virtual void end()final;
		void initialise();

	};
}