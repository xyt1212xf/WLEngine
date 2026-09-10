#include "TransparentRendererPass.h"
#include "WLEngine.h"

namespace WL
{

	CTransparentRendererPass::CTransparentRendererPass(CRenderer* pRenderer)
	: CRendererPass(pRenderer)
	{

	}

	CTransparentRendererPass::~CTransparentRendererPass()
	{

	}

	bool CTransparentRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return true;
	}

	void CTransparentRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{

	}

	void CTransparentRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{

	}

	std::vector<CActorEntity*>& CTransparentRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	void CTransparentRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{

	}

	bool CTransparentRendererPass::initlaliseMaterial()
	{
		return false;
	}

	void CTransparentRendererPass::initlaliseOutputRT()
	{

	}

}