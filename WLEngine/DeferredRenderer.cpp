#include "DeferredRenderer.h"
#include "WLEngine.h"

namespace WL
{
	CDeferredRenderer::CDeferredRenderer()
	{
		mOrder = DeferredRenderer;
#if 0
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\deferredRenderer.ma)", Material));
		mArrayMaterialIns[DrawQualityLevel::Middle] = WL_NEW(CMaterialInstance, MaterialInstance)(pMaterial);
#endif
		setQualityLevel(DrawQualityLevel::Middle);
		initialise();

	}

	CDeferredRenderer::~CDeferredRenderer()
	{
	}
		
	void CDeferredRenderer::initialise()
	{
		auto pDeferredPass = WL_NEW(CDeferredRendererPass, RendererPass)(this);
		auto pLightMeshPass = WL_NEW(CLightMeshRendererPass, RendererPass)(this);
		auto pDepthPass = WL_NEW(CDepthRendererPass, RendererPass)(this);
		auto pDeferredLightPass = WL_NEW(CDeferredLightRendererPass, RendererPass)(this);
		auto pShadowPass = WL_NEW(CShadowMapRendererPass, RendererPass)(this);
		auto pNoLightPass = WL_NEW(CNoLightRendererPass, RendererPass)(this);
		auto pGuiPass = WL_NEW(CGuiRendererPass, RendererPass)(this);

		if (pDeferredPass->initialise() &&
			pDepthPass->initialise() &&
			pLightMeshPass->initialise() &&
			pDeferredLightPass->initialise() &&
			pShadowPass->initialise() &&
			pNoLightPass->initialise() &&
			pGuiPass->initialise())
		{
			addRendererPass(pDeferredPass);
			addRendererPass(pLightMeshPass);

			addRendererPass(pDepthPass);
			addRendererPass(pDeferredLightPass);
			addRendererPass(pShadowPass);
			addRendererPass(pNoLightPass);
			addRendererPass(pGuiPass);

			mListRenderPass.sort(CRendererPass::RendererPassCompare());

			pDeferredLightPass->setInputRT(pDeferredPass->getArrayOutputRT());
			pShadowPass->addInputRT(pDeferredPass->getOutputRT(1));
			pShadowPass->addInputRT(pDeferredLightPass->getOutputRT(0));
			pShadowPass->addInputRT(pDepthPass->getOutputRT(0));

			WL_DECREASE(pDeferredPass);
			WL_DECREASE(pDepthPass);
			WL_DECREASE(pLightMeshPass);
			WL_DECREASE(pDeferredLightPass);
			WL_DECREASE(pShadowPass);
			WL_DECREASE(pNoLightPass);
			WL_DECREASE(pGuiPass);
		}

	}

	void CDeferredRenderer::drawEntity(RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
#if 0
		for (int i = nBegin; i < nBegin + nCount; ++i)
		{
			auto pActor = entities[i];
			RenderUnit* pRenderUnit = nullptr;
			for (auto item : pActor->getModels())
			{
				auto& meshsInfo = (item.second)->getAllMeshInfo();
				for (auto& child : meshsInfo)
				{
					if (nullptr != child.pMeshInstance)
					{
						child.pMeshInstance->updateMatrix();

						pRenderUnit = pRenderGroup->getRenderUnit();
						pRenderUnit->pMaterialInstance = mpCurrentMaterialIns;
						UINT16 nOrder = pRenderUnit->pMaterialInstance->getRenderOrder();

						Geometry geometry;
						geometry.pTextures = child.pMaterialInstance->getTexturesPtr();
						geometry.vertexSize = child.pMeshInstance->getVertexTypeSize();
						geometry.vertexOffset = child.pMeshInstance->getVertexOffset();
						geometry.indeiceType = child.pMeshInstance->getIndeiceType();
						geometry.indexCount = child.pMeshInstance->getIndexCount();
						geometry.pMatrix[WORLD_MT_0] = child.pMeshInstance->getTransposeMT();
						geometry.pMatrix[VIEW_MT] = child.pMeshInstance->getTransposeViewMT();
						geometry.pMatrix[PROJECT_MT] = child.pMeshInstance->getTransposeProjectMT();
						geometry.pMatrix[VIEW_PROJECT_MT] = child.pMeshInstance->getTransposeViweProjectMatrix();

						geometry.pVB = child.pMeshInstance->getVertexBuffer();
						geometry.pIB = child.pMeshInstance->getIndexBuffer();
						pRenderUnit->geometry[nOrder].emplace_back(geometry);
					}
				}
			}
		}
#endif
	}

	std::vector<CActorEntity*>& CDeferredRenderer::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		mDrawEntities.clear();
		for (auto item : pDrawEntitiesArray)
		{
			bool bLight = false;
			for (auto model : item->getModels())
			{
				auto& meshsInfo = (model.second)->getAllMeshInfo();
				for (auto& child : meshsInfo)
				{
					if (nullptr != child.pMeshInstance)
					{
						if (child.pMaterialInstance->isLight())
						{
							bLight = true;
							break;
						}
					}
				}
			}
			if (bLight)
			{
				mDrawEntities.emplace_back(item);
			}
		}
		return mDrawEntities;
	}

	bool CDeferredRenderer::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		//if (bCleanState)
		//{
		//	pDeviceContext->ClearState(); 
		//}
		//pDeviceContext->OMSetRenderTargets(mBufferSize, mArrayRenderTargetView.data(), mpDepthStencilView);
		//pDeviceContext->RSSetViewports(1, &Dev->getViewPort());
		return true;
	}

	void CDeferredRenderer::drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext)
	{
#if 0
		auto& renderUnits = mRenderChunkGroup[nContext].mpRenderUnits;
		size_t nRenderChunks = renderUnits.size();
		if (nRenderChunks > 0)
		{
			//commitToGpu(pDeviceContext, renderUnits);
			pDeviceContext->FinishCommandList(FALSE, &pCommandList);
			mRenderChunkGroup[nContext].clearRenderUnit();
		}
#endif
	}

	void CDeferredRenderer::begin()
	{		
		for (auto item : mListRenderPass)
		{
			item->clearSurface();
		}
	}

	void CDeferredRenderer::end()
	{
		CRenderer::end();
	}



}