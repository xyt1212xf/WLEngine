#include "DepthRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CDepthRendererPass::CDepthRendererPass(CRenderer* pRenderer)
		:Parent(pRenderer)
	{
		mOrder = eRendererPass::DepthPass;
	}

	CDepthRendererPass::~CDepthRendererPass()
	{
	}

	bool CDepthRendererPass::initialise()
	{
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
			initlaliseOutputRT();
		}
		return bValue;
	}

	bool CDepthRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\depth.ma)", Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			bValue = true;
		}
		return bValue;
	}

	void CDepthRendererPass::initlaliseOutputRT()
	{
		auto pConfig = GEngine->getWindowConfigPtr();
		auto pSurface = GEngine->createRenderTarget(RenderTargetType::DepthRTT,
			pConfig->nWidth,
			pConfig->nWidth,
			FORMAT_R32_FLOAT,
			FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, RESOURCE_MISC_UNKNOW);
		mpDepthStencilView = pSurface->getDepthStencilViewPtr();
		mArrayOutputRTT.emplace_back(pSurface);
		mArrayOutputRTTView.emplace_back(pSurface->getRenderTargetViewPtr());
		mOutputBufferSize = (int)mArrayOutputRTT.size();
		mViewPort.resize(1);
		memcpy(&mViewPort[0], &Dev->getViewPort(), sizeof(SViewPort));
		mViewPort[0].Height = static_cast<float>(pConfig->nWidth);
	}

	bool CDepthRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return Parent::drawBegin(pDeviceContext, bCleanState);
		//if (bCleanState)
		//{
		//	pDeviceContext->ClearState();
		//}
		//pDeviceContext->OMSetRenderTargets(1, &mpShadowRTTView, mpShadowDepthStencilView);
		//pDeviceContext->RSSetViewports(1, &mpShadowSurface->getViewPort());
		//return true;
	}

	void CDepthRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
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
						pRenderUnit->pMaterialInstance = mpMaterialIns;
						UINT16 nOrder = mpMaterialIns->getRenderOrder();

						SGeometry geometry;
						geometry.pMaterialInstance = mpMaterialIns;
						geometry.pTextures = child.pMaterialInstance->getTexturesPtr();
						geometry.vertexSize = child.pMeshInstance->getVertexTypeSize();
						geometry.vertexOffset = child.pMeshInstance->getVertexOffset();
						geometry.indexType = child.pMeshInstance->getIndeiceType();
						geometry.indexCount = child.pMeshInstance->getIndexCount();
						geometry.pMatrix[WORLD_MT_0] = child.pMeshInstance->getTransposeMT();
						geometry.pMatrix[VIEW_MT] = GEngine->getLightTransposeViewMT();
						geometry.pMatrix[PROJECT_MT] = GEngine->getLightTransposeProjectionMT();
						//geometry.pMatrix[VIEW_PROJECT_MT] = GEngine->getLightTransposeVPMT();
						geometry.startIndex = child.pMeshInstance->getStartIndexLocation();
						geometry.baseVertexLocation = child.pMeshInstance->getBaseVertexLocation();
						geometry.indexCount = child.pMeshInstance->getIndexCount();
						geometry.pVB = child.pMeshInstance->getVertexBuffer();
						geometry.pIB = child.pMeshInstance->getIndexBuffer();
						pRenderUnit->geometry[nOrder].emplace_back(geometry);
					}
				}
			}
		}
	}

	void CDepthRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CDepthRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		mDrawEntities.clear();
		for (auto item : pDrawEntitiesArray)
		{
			bool bShadow = false;
			for(auto& [k, v]: item->getModels())
			{
				if ((v)->isVisual())
				{
					auto& meshsInfo = v->getAllMeshInfo();
					for (auto& child : meshsInfo)
					{
						if (nullptr != child.pMeshInstance)
						{
							if (child.pMaterialInstance->isShadow())
							{
								bShadow = true;
								break;
							}
						}
					}
				}
			}
			if (bShadow)
			{
				mDrawEntities.emplace_back(item);
			}
		}
		return mDrawEntities;
	}

	void CDepthRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			drawGeometry(item, pDeviceContext);
		}
	}
}