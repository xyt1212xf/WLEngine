#include "DeferredRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CDeferredRendererPass::CDeferredRendererPass(CRenderer* pRenderer)
		: Parent(pRenderer)
	{
		mOrder = eRendererPass::ActorDrawPass;
	}

	CDeferredRendererPass::~CDeferredRendererPass()
	{
		//	mArrayRenderTargetView.clear();
	}

	bool CDeferredRendererPass::initialise()
	{
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
			initlaliseOutputRT();
			//	mBufferSize = (int)mArrayRenderTargetView.size();
		}
		return bValue;
	}


	bool CDeferredRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\deferredRenderer.ma)", Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			bValue = true;
		}
		return bValue;
	}

	void CDeferredRendererPass::initlaliseOutputRT()
	{
		auto pConfig = GEngine->getWindowConfigPtr();
		mArrayOutputRTT.emplace_back(GEngine->createRenderTarget(RenderTargetType::ColorRTT,
			pConfig->nWidth,
			pConfig->nHeight,
			FORMAT_R32G32B32A32_FLOAT,
			0, 0, RESOURCE_MISC_UNKNOW));
		mArrayOutputRTT.emplace_back(GEngine->createRenderTarget(RenderTargetType::PositionRTT,
			pConfig->nWidth,
			pConfig->nHeight,
			FORMAT_R32G32B32A32_FLOAT,
			0, 0, RESOURCE_MISC_UNKNOW));
		mArrayOutputRTT.emplace_back(GEngine->createRenderTarget(RenderTargetType::NormalRTT,
			pConfig->nWidth,
			pConfig->nHeight,
			FORMAT_R32G32B32A32_FLOAT,
			0, 0, RESOURCE_MISC_UNKNOW));
		for (auto item : mArrayOutputRTT)
		{
			mArrayOutputRTTView.emplace_back(item->getRenderTargetViewPtr());
		}
		mpDepthStencilView = Dev->getDepthStencilView();
		mOutputBufferSize = (int)mArrayOutputRTT.size();
		mViewPort.resize(mOutputBufferSize);
		for (size_t i = 0; i < mViewPort.size(); i++)
		{
			memcpy(&mViewPort[i], &Dev->getViewPort(), sizeof(SViewPort));
		}

	}

	bool CDeferredRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		//if (bCleanState)
		//{
		//	pDeviceContext->ClearState();
		//}
		//pDeviceContext->OMSetRenderTargets(mBufferSize, mArrayRenderTargetView.data(), mpDepthStencilView);
		//pDeviceContext->RSSetViewports(1, &Dev->getViewPort());

		return Parent::drawBegin(pDeviceContext, bCleanState);
	}

	void CDeferredRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
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
						geometry.pMatrix[VIEW_MT] = child.pMeshInstance->getTransposeViewMT();
						geometry.pMatrix[PROJECT_MT] = child.pMeshInstance->getTransposeProjectMT();
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


	void CDeferredRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CDeferredRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
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

	void CDeferredRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			drawGeometry(item, pDeviceContext);
		}
	}

}