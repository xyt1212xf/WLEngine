#include "RendererPass.h"
#include "Renderer.h"
#include "WLEngine.h"
#include "SurfaceView.h"

namespace WL
{
	CRendererPass::CRendererPass(CRenderer* pRenderer)
	: mpRenderer(pRenderer)
	{
	}

	CRendererPass::~CRendererPass()
	{
		WL_DELETE(mpMaterialIns, Instance);
		for (auto item : mArrayInputRTT)
		{
			WL_DECREASE(item);
		}
		mArrayInputRTT.clear();
		for (auto item : mArrayOutputRTT)
		{
			WL_DECREASE(item);
		}
		mArrayOutputRTT.clear();
	}

	bool CRendererPass::initialise()
	{	
		bool bValue = initlaliseMaterial();
		if (bValue)
		{
			initlaliseOutputRT();
		}
		return bValue;
	}

	void CRendererPass::initlaliseOutputRT()
	{
	}

	void CRendererPass::drawFinal()
	{

	}

	void CRendererPass::initDefaultOutputRT()
	{
		auto pRTTView = Dev->getRenderTargetView();
		auto pDetenStencilView = Dev->getDepthStencilView();
		CSurfaceView* pSurface = WL_NEW(CSurfaceView, SurfaceView)(pRTTView, pDetenStencilView);
		mArrayOutputRTT.push_back(pSurface);
		mArrayOutputRTTView.emplace_back(pSurface->getRenderTargetViewPtr());
		mpDepthStencilView = Dev->getDepthStencilView();
		mOutputBufferSize = 1;
		mViewPort.resize(1);
		memcpy(&mViewPort[0], &Dev->getViewPort(), sizeof(SViewPort));
	}

	bool CRendererPass::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		if (bCleanState)
		{
			pDeviceContext->ClearState();
		}
		pDeviceContext->OMSetRenderTargets(mOutputBufferSize, mArrayOutputRTTView.data(), mpDepthStencilView);
		pDeviceContext->RSSetViewports(mOutputBufferSize, (D3D11_VIEWPORT*)(mViewPort.data()));
		return true;
	}

	void CRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
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
						geometry.startIndex = child.pMeshInstance->getStartIndexLocation();
						geometry.baseVertexLocation = child.pMeshInstance->getBaseVertexLocation();

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
	}

	void CRendererPass::drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext)
	{
		auto& renderUnits = pRenderGroup->mpRenderUnits;
		size_t nRenderChunks = renderUnits.size();
		if (nRenderChunks > 0)
		{
			commitToGpu(pDeviceContext, pRenderGroup);
			pRenderGroup->clearRenderUnit();
			pDeviceContext->FinishCommandList(FALSE, &pCommandList);		
		}
	}

	std::vector<CActorEntity*>& CRendererPass::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	UINT32 CRendererPass::getOrder() const
	{
	//	assert(-1 != mOrder);
		return (int)mOrder;
	}

	bool CRendererPass::operator<(const CRendererPass* r)
	{
		return this->mOrder < r->mOrder;
	}

	void CRendererPass::setInputRT(std::vector<CSurfaceView*>& arrayRenderSurface)
	{
		for (auto item : arrayRenderSurface)
		{
			item->addRef();
			mArrayInputRTT.emplace_back(item);
			if (nullptr != mpMaterialIns)
			{
				auto pSample = mpMaterialIns->getMaterial()->getSampler();
				if (nullptr != pSample)
				{
					auto pTexture = item->getTexturePtr();
					if (nullptr != pTexture)
					{
						pTexture->setSampleState(pSample);
					}
				}
			}
		}
	}

	void CRendererPass::addInputRT(CSurfaceView* pRenderSurface)
	{
		if (nullptr != pRenderSurface)
		{
			pRenderSurface->addRef();
			if (nullptr != mpMaterialIns)
			{
				auto pSample = mpMaterialIns->getMaterial()->getSampler();
				if (nullptr != pSample)
				{
					auto pTexture = pRenderSurface->getTexturePtr();
					if (nullptr != pTexture && nullptr == pTexture->getSampleState())
					{
						pTexture->setSampleState(pSample);
					}
				}
			}
			mArrayInputRTT.emplace_back(pRenderSurface);
		}
	}

	std::vector<CSurfaceView*>& CRendererPass::getArrayOutputRT()
	{
		return mArrayOutputRTT;
	}

	CSurfaceView* CRendererPass::getOutputRT(int nIndex)
	{
		CSurfaceView* pSurface = nullptr;
		if (nIndex < (int)mArrayOutputRTT.size())
		{
			pSurface = mArrayOutputRTT[nIndex];
		}
		return pSurface;
	}

	void CRendererPass::clearSurface()
	{
		// Clear the back buffer.
		for (auto item : mArrayOutputRTTView)
		{
			GEngine->getImmediateContext()->ClearRenderTargetView(item, mClearColor.data());
		}
	}

	void CRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		//for (auto& item : pRenderUnitGroup->mpRenderUnits)
		//{
		//	if (item->pMaterialInstance->isLight())
		//	{
		//		GEngine->setAllLights(item->pMaterialInstance);
		//	}
		//	if (item->pMaterialInstance->isShadow())
		//	{
		//		auto pRenderSurface = GEngine->getRenderTarget(DepthRTT);
		//		if (nullptr != pRenderSurface)
		//		{
		//			pRenderSurface->applyTexture(pDeviceContext, DepthRTT);
		//		}
		//	}
		//	drawGeometry(item, pDeviceContext);
		//}
	}

	bool CRendererPass::initlaliseMaterial()
	{
		return false;
	}

	void CRendererPass::drawGeometry(SRenderUnit* pItem, DeviceContext* pDeviceContext)
	{
		for (auto& [key, value] : pItem->geometry)
		{
			for (auto& child : value)
			{
				{
					CMutex::CAutoLock lock(mMutex);
					(child).pMaterialInstance->updateVSConstBuffer(0, (child).pMatrix->data());
					(child).pMaterialInstance->commitToGpu(pDeviceContext);
				}
				for (auto texture : *child.pTextures)
				{
					if (nullptr != texture.second)
					{
						(texture.second)->use(pDeviceContext, texture.first);
					}
				}
				auto pVB = (child).pVB->getBuffer();
				auto pIB = (child).pIB->getBuffer();

				pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &(child).vertexSize, &(child).vertexOffset);
				// Set the index buffer to active in the input assembler so it can be rendered.
				pDeviceContext->IASetIndexBuffer(pIB, (DXGI_FORMAT)(child).indexType, 0);

				pDeviceContext->DrawIndexed((child).indexCount, (child).startIndex, (child).baseVertexLocation);
			}
		}
	}
}