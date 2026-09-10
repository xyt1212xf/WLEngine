#include "ShadowMapRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CShadowMapRendererPass::CShadowMapRendererPass(CRenderer* pRenderer)
		:Parent(pRenderer)
	{
		mOrder = eRendererPass::ShadowPass;
	}

	CShadowMapRendererPass::~CShadowMapRendererPass()
	{
		mDrawEntities.clear();
		WL_DELETE(mpFullScreenActor, Entity);
	}

	bool CShadowMapRendererPass::initialise()
	{
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
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
				initDefaultOutputRT();
			}
			else
			{
				bValue = false;
			}
		}
		return bValue;
	}


	bool CShadowMapRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\shadow.ma)", Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			auto pConfig = GEngine->getWindowConfigPtr();
			auto pSurface = GEngine->createRendererSurface(pConfig->nWidth, pConfig->nHeight, FORMAT_R32G32B32A32_FLOAT, 0, 0, RESOURCE_MISC_UNKNOW);
			pSurface->bindDepthStencilView(Dev->getDepthStencilView());

			bValue = true;
		}
		return bValue;
	}

	bool CShadowMapRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		Parent::drawBegin(pDeviceContext, bCleanState);
		mpMaterialIns->setPSParamByName("lightViewMatrix", GEngine->getLightTransposeViewMT().data());
		mpMaterialIns->setPSParamByName("lightProjectionMatrix", GEngine->getLightTransposeProjectionMT().data());
		size_t nSize = mArrayInputRTT.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			mArrayInputRTT[i]->getTexturePtr()->use(pDeviceContext, i);
		}
		return true;
	}

	void CShadowMapRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
		SRenderUnit* pRenderUnit = nullptr;
		auto item = mpFullScreenActor->getModels().begin();
		auto& meshsInfo = (item->second)->getAllMeshInfo().front();
		if (nullptr != meshsInfo.pMeshInstance)
		{
			meshsInfo.pMeshInstance->updateMatrix();

			pRenderUnit = pRenderGroup->getRenderUnit();
			pRenderUnit->pMaterialInstance = mpMaterialIns;
			UINT16 nOrder = mpMaterialIns->getRenderOrder();

			SGeometry geometry;
			geometry.pMaterialInstance = mpMaterialIns;
			geometry.pTextures = mpMaterialIns->getTexturesPtr();
			geometry.vertexSize = meshsInfo.pMeshInstance->getVertexTypeSize();
			geometry.vertexOffset = meshsInfo.pMeshInstance->getVertexOffset();
			geometry.indexType = meshsInfo.pMeshInstance->getIndeiceType();
			geometry.indexCount = meshsInfo.pMeshInstance->getIndexCount();
			geometry.pMatrix[WORLD_MT_0] = meshsInfo.pMeshInstance->getTransposeMT();
			geometry.pMatrix[VIEW_MT] = meshsInfo.pMeshInstance->getTransposeViewMT();
			geometry.pMatrix[PROJECT_MT] = meshsInfo.pMeshInstance->getTransposeProjectMT();
			geometry.startIndex = meshsInfo.pMeshInstance->getStartIndexLocation();
			geometry.baseVertexLocation = meshsInfo.pMeshInstance->getBaseVertexLocation();
			geometry.pVB = meshsInfo.pMeshInstance->getVertexBuffer();
			geometry.pIB = meshsInfo.pMeshInstance->getIndexBuffer();
			pRenderUnit->geometry[nOrder].emplace_back(geometry);
		}
	}

	void CShadowMapRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CShadowMapRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	void CShadowMapRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			drawGeometry(item, pDeviceContext);
		}
	}

}