#include "DeferredLightRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CDeferredLightRendererPass::CDeferredLightRendererPass(CRenderer* pRenderer)
		: Parent(pRenderer)
	{
		mOrder = eRendererPass::DeferredLightPass;
	}

	CDeferredLightRendererPass::~CDeferredLightRendererPass()
	{
		mDrawEntities.clear();
		WL_DELETE(mpFullScreenActor, Entity);
		WL_DELETE(mpStencilIns, Instance);
	}

	bool CDeferredLightRendererPass::initialise()
	{
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
			initlaliseOutputRT();
			auto pConfig = GEngine->getWindowConfigPtr();
			mpFullScreenActor = GEngine->createBillBoardEntity(R"(resource\Material\drawScreen.ma)", pConfig->nWidth, pConfig->nHeight);
			if (nullptr != mpFullScreenActor)
			{
				Math::buildOrthographicMatrixLH(&mFullScreenMT, (float)pConfig->nWidth, (float)pConfig->nHeight, 1.0f, 1000.0f);
				Math::matrixTranspose(&mFullScreenTransposeMT, &mFullScreenMT);

				mpFullScreenActor->setPosition(Vec3F(0, 0, 0));
				mpFullScreenActor->setViewMT(const_cast<SMatrix44*>(&SMatrix44::identity()));
				mpFullScreenActor->setProjectMT(&mFullScreenMT);

				mpFullScreenActor->setTransposeViewMT(const_cast<SMatrix44*>(&SMatrix44::identity()));
				mpFullScreenActor->setTransposeProjectMT(&mFullScreenTransposeMT);

				mDrawEntities.emplace_back(mpFullScreenActor);

				SRenderStateDesc renderStateDesc;
				renderStateDesc.rasterizerDesc.CullMode = CULL_NONE;

				renderStateDesc.depthStencilDesc.DepthEnable = FALSE;
				renderStateDesc.depthStencilDesc.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;

				renderStateDesc.depthStencilDesc.StencilEnable = TRUE;
				renderStateDesc.depthStencilDesc.FrontFace.StencilFunc = COMPARISON_NOT_EQUAL;
				renderStateDesc.depthStencilDesc.BackFace.StencilFunc = COMPARISON_NOT_EQUAL;
				auto pRenderState = Dev->createRenderState(renderStateDesc);
				if (nullptr != pRenderState)
				{
					mpStencilIns->bindRenderState(pRenderState);
				}
			}
			else
			{
				bValue = false;
			}
		}
		return bValue;
	}


	bool CDeferredLightRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\deferredLight.ma)", Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			bValue = true;

			pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\deferredStencil.ma)", Material));
			mpStencilIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			bValue = true;
		}
		return bValue;
	}

	void CDeferredLightRendererPass::initlaliseOutputRT()
	{
		auto pConfig = GEngine->getWindowConfigPtr();
		auto pSurface = GEngine->createRendererSurface(pConfig->nWidth,
			pConfig->nHeight,
			FORMAT_R32G32B32A32_FLOAT,
			0, 0, 0);
		mArrayOutputRTT.emplace_back(pSurface);
		mArrayOutputRTTView.emplace_back(pSurface->getRenderTargetViewPtr());
		mpDepthStencilView = Dev->getDepthStencilView();
		mOutputBufferSize = (int)mArrayOutputRTT.size();
		mViewPort.resize(1);
		memcpy(&mViewPort[0], &Dev->getViewPort(), sizeof(SViewPort));
	}

	bool CDeferredLightRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		Parent::drawBegin(pDeviceContext, bCleanState);
		size_t nSize = mArrayInputRTT.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			mArrayInputRTT[i]->getTexturePtr()->use(pDeviceContext, i);
		}
		return true;
		//if (bCleanState)
		//{
		//	pDeviceContext->ClearState();
		//}
		//auto pDevice = GEngine->getDevice();
		//auto pRenderTargetView = pDevice->getRenderTargetView();
		//pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDevice->getDepthStencilView());
		//pDeviceContext->RSSetViewports(1, &pDevice->getViewPort());
		//return true;
	}

	void CDeferredLightRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
		SRenderUnit* pRenderUnit = nullptr;
		auto item = mpFullScreenActor->getModels().begin();
		auto& meshsInfo = (item->second)->getAllMeshInfo().front();
		if (nullptr != meshsInfo.pMeshInstance)
		{
			auto matProcess = [&](CMaterialInstance* pMaterialInstance)
			{
				meshsInfo.pMeshInstance->updateMatrix();

				pRenderUnit = pRenderGroup->getRenderUnit();
				pRenderUnit->pMaterialInstance = pMaterialInstance;
				UINT16 nOrder = pMaterialInstance->getRenderOrder();

				SGeometry geometry;
				geometry.pMaterialInstance = pMaterialInstance;
				geometry.pTextures = pMaterialInstance->getTexturesPtr();
				geometry.vertexSize = meshsInfo.pMeshInstance->getVertexTypeSize();
				geometry.vertexOffset = meshsInfo.pMeshInstance->getVertexOffset();
				geometry.indexType = meshsInfo.pMeshInstance->getIndeiceType();
				geometry.indexCount = meshsInfo.pMeshInstance->getIndexCount();
				geometry.pMatrix[WORLD_MT_0] = meshsInfo.pMeshInstance->getTransposeMT();
				geometry.pMatrix[VIEW_MT] = meshsInfo.pMeshInstance->getTransposeViewMT();
				geometry.pMatrix[PROJECT_MT] = meshsInfo.pMeshInstance->getTransposeProjectMT();
				//	geometry.pMatrix[VIEW_PROJECT_MT] = child.pMeshInstance->getTransposeViweProjectMatrix();
				geometry.startIndex = meshsInfo.pMeshInstance->getStartIndexLocation();
				geometry.baseVertexLocation = meshsInfo.pMeshInstance->getBaseVertexLocation();
				geometry.pVB = meshsInfo.pMeshInstance->getVertexBuffer();
				geometry.pIB = meshsInfo.pMeshInstance->getIndexBuffer();
				pRenderUnit->geometry[nOrder].emplace_back(geometry);
			};
			matProcess(mpMaterialIns);
			matProcess(mpStencilIns);
		}
	}


	void CDeferredLightRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CDeferredLightRendererPass::checkEntities([[maybe_unused]] std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	void CDeferredLightRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		//if(pRenderUnitGroup->mpRenderUnits.size() > 0 )
		//{		
		//	auto item = pRenderUnitGroup->mpRenderUnits.front();
		//	GEngine->setAllLights(item->pMaterialInstance);

		//	drawGeometry(item, pDeviceContext);
		//}
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			GEngine->setAllLights(item->pMaterialInstance);
			drawGeometry(item, pDeviceContext);
		}
	}
}