#include "LightMeshRendererPass.h"
#include "WLEngine.h"

namespace WL
{
	CLightMeshRendererPass::CLightMeshRendererPass(CRenderer* pRenderer)
		: CRendererPass(pRenderer)
	{
		mOrder = eRendererPass::LightMeshPass;
	}


	CLightMeshRendererPass::~CLightMeshRendererPass()
	{
	}

	bool CLightMeshRendererPass::initialise()
	{
		mpDepthStencilView = Dev->getDepthStencilView();
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
			SRenderStateDesc renderStateDesc;
			renderStateDesc.rasterizerDesc.CullMode = CULL_NONE;

			renderStateDesc.depthStencilDesc.DepthEnable = TRUE;
			renderStateDesc.depthStencilDesc.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
			renderStateDesc.depthStencilDesc.StencilEnable = TRUE;
			renderStateDesc.depthStencilDesc.FrontFace.StencilDepthFailOp = STENCIL_OP_DECR;
			renderStateDesc.depthStencilDesc.FrontFace.StencilPassOp = STENCIL_OP_KEEP;
			renderStateDesc.depthStencilDesc.BackFace.StencilDepthFailOp = STENCIL_OP_INCR;
			renderStateDesc.depthStencilDesc.BackFace.StencilPassOp = STENCIL_OP_KEEP;
			auto pRenderState = Dev->createRenderState(renderStateDesc);

			mpMaterialIns->bindRenderState(pRenderState);
			initDefaultOutputRT();
		}
		return bValue;
	}

	bool CLightMeshRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(R"(resource\Material\stencilTest.ma)", Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			bValue = true;
		}
		return bValue;
	}

	bool CLightMeshRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return Parent::drawBegin(pDeviceContext, bCleanState);
	}

	void CLightMeshRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
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

	void CLightMeshRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pRenderGroup, pCommandList, nContext);
	}

	std::vector<CActorEntity*>& CLightMeshRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		mDrawEntities.clear();
		for (auto item : pDrawEntitiesArray)
		{
			if (dynamic_cast<CLightModel*>(item))
			{
				mDrawEntities.emplace_back(item);
			}
		}
		return mDrawEntities;
	}

	void CLightMeshRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			drawGeometry(item, pDeviceContext);
		}
	}

	void CLightMeshRendererPass::clearSurface()
	{
	}
}