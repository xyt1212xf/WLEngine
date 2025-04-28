#include "Renderer.h"
#include "WLEngine.h"
#include <algorithm>

namespace WL
{
	RenderUnit* RenderUnitGrounp::getRenderUnit()
	{
		RenderUnit* pRenderUnit = nullptr;
		if (mFreeRenderUnits.size() > 0)
		{
			pRenderUnit = mFreeRenderUnits.front();
			mFreeRenderUnits.pop_front();
		}
		else
		{
			pRenderUnit = WL_NEW(RenderUnit, RenderUnit);
		}
		mpRenderUnits.emplace_back(pRenderUnit);
		return pRenderUnit;
	}

	void RenderUnitGrounp::clearRenderUnit()
	{
		for (auto item : mpRenderUnits)
		{
			item->geometry.clear();
			mFreeRenderUnits.emplace_back(item);
		}
		mpRenderUnits.clear();
	}

	RenderUnitGrounp::~RenderUnitGrounp()
	{	
		for (auto item : mpRenderUnits)
		{
			WL_DELETE(item, RenderUnit);
		}
		mpRenderUnits.clear();
		for (auto item : mFreeRenderUnits)
		{
			WL_DELETE(item, RenderUnit);
		}
		mFreeRenderUnits.clear();

	}

	//////////////////////////////////////////////////////////////////////////
	//const static int sMaxRenderChunk = 32;
	//RenderChunkGrounp gRenderChunkGroup[sMaxRenderChunk];

	CRenderer::CRenderer()
	{
	}

	CRenderer::CRenderer(int nOrder)
	{
		mOrder = nOrder;
	}

	CRenderer::~CRenderer()
	{
		for (auto item : mListRenderPass)
		{
			WL_DECREASE(item);
		}
		mArrayRendererPass.clear();
		mListRenderPass.clear();
	}


	RenderUnitGrounp* CRenderer::getRenderUnitGrounp(INT32 nIndex)
	{
#ifdef _DEBUG
		if (nIndex < sMaxRenderChunk)
		{
			return &mRenderChunkGroup[nIndex];
		}
		else
		{
			return nullptr;
		}
#else
		return &mRenderChunkGroup[nIndex];
#endif
	}

	int CRenderer::getOrder()
	{
		return mOrder;
	}

	std::vector<CActorEntity*>& CRenderer::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		return mDrawEntities;
	}

	void CRenderer::drawEntity(RenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
		for (int i = nBegin; i < nBegin + nCount; ++i)
		{
			auto pActor = entities[i];
			RenderUnit* pRenderUnit = nullptr;
			for (auto item : pActor->getModels())
			{
				if ((item.second)->isVisual())
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

							Geometry geometry;
							geometry.pMaterialInstance = child.pMaterialInstance;
							geometry.vertexSize = child.pMeshInstance->getVertexTypeSize();
							geometry.vertexOffset = child.pMeshInstance->getVertexOffset();
							geometry.indexType = child.pMeshInstance->getIndeiceType();
							geometry.indexCount = child.pMeshInstance->getIndexCount();
							geometry.pMatrix[WORLD_MT_0] = child.pMeshInstance->getTransposeMT();
							geometry.pMatrix[VIEW_MT] = child.pMeshInstance->getTransposeViewMT();
							geometry.pMatrix[PROJECT_MT] = child.pMeshInstance->getTransposeProjectMT();
							geometry.pMatrix[VIEW_PROJECT_MT] = child.pMeshInstance->getTransposeViweProjectMatrix();
							geometry.pMatrix[WORLD_MT_1] = GEngine->getLightTransposeViewMT();
							geometry.pMatrix[WORLD_MT_2] = GEngine->getLightTransposeProjectionMT();
							geometry.pVB = child.pMeshInstance->getVertexBuffer();
							geometry.pIB = child.pMeshInstance->getIndexBuffer();
							pRenderUnit->geometry[nOrder].emplace_back(geometry);

						}
					}
				}
			}
		}
	}

	bool CRenderer::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		if (bCleanState)
		{
			pDeviceContext->ClearState();
		}
		auto pDevice = GEngine->getDevice();
		auto pRenderTargetView = pDevice->getRenderTargetView();
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDevice->getDepthStencilView());
		pDeviceContext->RSSetViewports(1, &pDevice->getViewPort());
		return true;
	}

	void CRenderer::drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext)
	{
		auto& renderUnits = mRenderChunkGroup[nContext].mpRenderUnits;
		size_t nRenderChunks = renderUnits.size();
		if (nRenderChunks > 0)
		{
			//commitToGpu(pDeviceContext, renderUnits);
			pDeviceContext->FinishCommandList(FALSE, &pCommandList);
			mRenderChunkGroup[nContext].clearRenderUnit();
		}
	}

	void CRenderer::begin()
	{
	}

	void CRenderer::end()
	{
		for( auto item : mListRenderPass)
		{
			item->drawFinal();
		}
	}

	void CRenderer::setQualityLevel(DrawQualityLevel nLevle)
	{
		auto iter = mArrayRendererPass.find(nLevle);
		if (iter != mArrayRendererPass.end())
		{
			mDrawQualityLevel = nLevle;
			mListRenderPass = iter->second;
		}
	}

	DrawQualityLevel CRenderer::getQualityLevel() const
	{
		return mDrawQualityLevel;
	}

	void CRenderer::addRendererPass(CRendererPass* pPass)
	{
		auto iter = std::find(mListRenderPass.end(), mListRenderPass.end(), pPass);
		if (mListRenderPass.end() == iter)
		{
			pPass->addRef();
			mListRenderPass.emplace_back(pPass);
		}
	}

	void CRenderer::removeRendererPass(CRendererPass* pPass)
	{
		auto iter = std::find(mListRenderPass.end(), mListRenderPass.end(), pPass);
		if (iter != mListRenderPass.end())
		{
			WL_DECREASE((*iter));
			mListRenderPass.erase(iter);
		}
	}

#ifdef _DEBUG
	void CRenderer::setClearColor(const Vec4F& color )
	{
		mBackColor = color;
	}
#endif

	void CRenderer::commitToGpu(DeviceContext* pDeviceContext, std::list<RenderUnit*>& renderChunk)
	{
		for (auto& item : renderChunk)
		{
			//if (item->pMaterialInstance->isLight())
			//{
			//	GEngine->setAllLights(item->pMaterialInstance);
			//}
			//if (item->pMaterialInstance->isShadow())
			//{
			//	auto pRenderSurface = GEngine->getRenderTarget(DepthRTT);
			//	if (nullptr != pRenderSurface)
			//	{
			//		pRenderSurface->applyTexture(pDeviceContext, DepthRTT);
			//	}
			//}
			for( auto _item : item->geometry)
			{
				for (auto& child : _item.second)
				{
					for (auto texture : *child.pTextures)
					{
						if (nullptr != texture.second)
						{
							(texture.second)->use(pDeviceContext, texture.first);
						}
					}
					(child).pMaterialInstance->updateVSConstBuffer(0, (child).pMatrix->data());
					(child).pMaterialInstance->commitToGpu(pDeviceContext);
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
}