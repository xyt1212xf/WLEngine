#include "NoLightRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CNoLightRendererPass::CNoLightRendererPass(CRenderer* pRenderer)
		:Parent(pRenderer)
	{
		mOrder = eRendererPass::NoLightActorPass;
	}

	CNoLightRendererPass::~CNoLightRendererPass()
	{

	}

	bool CNoLightRendererPass::initialise()
	{
		initDefaultOutputRT();
		return true;
	}

	bool CNoLightRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return Parent::drawBegin(pDeviceContext, bCleanState);
		//if (bCleanState)
		//{
		//	pDeviceContext->ClearState();
		//}
		//auto pOutoutRtt = mArrayOutputRTT.front();
		//auto pRenderTargetView = pOutoutRtt->getRenderTargetView();
		//pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pOutoutRtt->getDepthStencilView());
		//pDeviceContext->RSSetViewports(1, &pOutoutRtt->getViewPort());
		//return true;
	}

	void CNoLightRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
		for (int i = nBegin; i < nBegin + nCount; ++i)
		{
			auto pActor = entities[i];
			SRenderUnit* pRenderUnit = nullptr;
			for (auto item : pActor->getModels())
			{
				auto& meshsInfo = (item.second)->getAllMeshInfo();
				for (auto& child : meshsInfo)
				{
					if (nullptr != child.pMeshInstance)
					{
						child.pMeshInstance->updateMatrix();

						pRenderUnit = pRenderGroup->getRenderUnit();
						pRenderUnit->pMaterialInstance = child.pMaterialInstance;
						UINT16 nOrder = child.pMaterialInstance->getRenderOrder();

						SGeometry geometry;
						geometry.pMaterialInstance = child.pMaterialInstance;
						geometry.pTextures = child.pMaterialInstance->getTexturesPtr();
						geometry.vertexSize = child.pMeshInstance->getVertexTypeSize();
						geometry.vertexOffset = child.pMeshInstance->getVertexOffset();
						geometry.indexType = child.pMeshInstance->getIndeiceType();
						geometry.indexCount = child.pMeshInstance->getIndexCount();
						geometry.pMatrix[WORLD_MT_0] = child.pMeshInstance->getTransposeMT();
						geometry.pMatrix[VIEW_MT] = child.pMeshInstance->getTransposeViewMT();
						geometry.pMatrix[PROJECT_MT] = child.pMeshInstance->getTransposeProjectMT();
						//	geometry.pMatrix[VIEW_PROJECT_MT] = child.pMeshInstance->getTransposeViweProjectMatrix();
						geometry.startIndex = child.pMeshInstance->getStartIndexLocation();
						geometry.baseVertexLocation = child.pMeshInstance->getBaseVertexLocation();
						geometry.pVB = child.pMeshInstance->getVertexBuffer();
						geometry.pIB = child.pMeshInstance->getIndexBuffer();
						pRenderUnit->geometry[nOrder].emplace_back(geometry);
					}
				}
			}
		}
	}

	void CNoLightRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CNoLightRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		mDrawEntities.clear();
		for (auto item : pDrawEntitiesArray)
		{
			if (!dynamic_cast<CLightModel*>(item))
			{
				bool bLight = true;
				for (auto model : item->getModels())
				{
					auto& meshsInfo = (model.second)->getAllMeshInfo();
					for (auto& child : meshsInfo)
					{
						if (nullptr != child.pMeshInstance)
						{
							if (!child.pMaterialInstance->isLight())
							{
								bLight = false;
								break;
							}
						}
					}
				}
				if (!bLight)
				{
					mDrawEntities.emplace_back(item);
				}
			}
		}
		return mDrawEntities;
	}

	void CNoLightRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			drawGeometry(item, pDeviceContext);
		}
	}
}