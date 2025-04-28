#include "SoftShadowRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CSoftShadowRendererPass::CSoftShadowRendererPass(CRenderer* pRenderer)
	: CRendererPass(pRenderer)
	{
		mClearColor = Vec4F(1, 1, 1, 1);
	}

	CSoftShadowRendererPass::~CSoftShadowRendererPass()
	{

	}

	bool CSoftShadowRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return Parent::drawBegin(pDeviceContext, bCleanState);
	}

	void CSoftShadowRendererPass::drawEntity(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{

	}

	void CSoftShadowRendererPass::drawEnd(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CSoftShadowRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	void CSoftShadowRendererPass::commitToGpu(DeviceContext* pDeviceContext, RenderUnitGrounp* pRenderUnitGroup)
	{

	}

	bool CSoftShadowRendererPass::initlaliseMaterial()
	{
		return false;
	}

	void CSoftShadowRendererPass::initlaliseOutputRT()
	{

	}

};