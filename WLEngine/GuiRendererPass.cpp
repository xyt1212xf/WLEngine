#include "GuiRendererPass.h"
#include "GuiEntity.h"
#include "WLEngine.h"
#include "Scene.h"
#include "GuiEngine.h"
#include "UILabel.h"

namespace WL
{
	int CGuiRendererPass::nMaxCount = 40960;
	const std::string CGuiRendererPass::szMaterName = R"(resource\Material\guiRenderer.ma)";
	const std::string CGuiRendererPass::szfontMaterName = R"(resource\Material\fontRenderer.ma)";
	CGuiRendererPass::CGuiRendererPass(CRenderer* pRenderer)
	: CRendererPass(pRenderer)
	{
		mOrder = eRendererPass::GuiPass;
	}

	CGuiRendererPass::~CGuiRendererPass()
	{
		WL_DELETE(mpVBBuffer, GpuBuffer);
		WL_DELETE(mpIBBuffer, GpuBuffer);
		WL_DELETE(mpMaterialIns, Instance);
		WL_DELETE(mpFontMaterialIns, Instance);		
	}

	bool CGuiRendererPass::initialise()
	{
		try
		{
			if (nullptr == mpVBBuffer)
			{
				mpVBBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
				mpVBBuffer->setType(USAGE_DYNAMIC);
				D3D11_BUFFER_DESC vertexBufferDesc;
				D3D11_SUBRESOURCE_DATA vertexData;
				ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
				ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
				vertexBufferDesc.Usage = static_cast<D3D11_USAGE>(USAGE_DYNAMIC);
				vertexBufferDesc.ByteWidth = sizeof(SVertexVTC) * nMaxCount;
				vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				mVertics.resize(nMaxCount);
				mDataLength = sizeof(SVertexVTC) * mVertics.size();
				vertexData.pSysMem = mVertics.data();
				vertexData.SysMemPitch = 0;
				vertexData.SysMemSlicePitch = 0;

				CDevice* pDevice = GEngine->getDevice();
				HRESULT result = pDevice->getGraphicsDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &mpVBBuffer->mpBuffer);
				if (FAILED(result))
				{
					mVertics.resize(0);
					throw false;
				}
			}
			if (nullptr == mpIBBuffer && mVertics.size() > 0)
			{
				int nCount = mVertics.size() / 4 * 6;
				mpIBBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
				D3D11_BUFFER_DESC indexBufferDesc;
				D3D11_SUBRESOURCE_DATA indexData;
				ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
				ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
				indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				indexBufferDesc.ByteWidth = sizeof(UINT16) * nCount;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = 0;
				indexBufferDesc.MiscFlags = 0;
				indexBufferDesc.StructureByteStride = 0;
				
				mIndices.resize(nCount);

				INT16 offset = 0;
				for( int i = 0; i < nCount; i += 6 )
				{
					mIndices[i + 0] = 0 + offset;
					mIndices[i + 1] = 1 + offset;
					mIndices[i + 2] = 2 + offset;

					mIndices[i + 3] = 1 + offset;
					mIndices[i + 4] = 3 + offset;
					mIndices[i + 5] = 2 + offset;

					offset += 4;
				}
				indexData.pSysMem = mIndices.data();
				indexData.SysMemPitch = 0;
				indexData.SysMemSlicePitch = 0;

				CDevice* pDevice = GEngine->getDevice();
				HRESULT result = pDevice->getGraphicsDevice()->CreateBuffer(&indexBufferDesc, &indexData, &mpIBBuffer->mpBuffer);
				if (FAILED(result))
				{
					throw false;
				}
			}
		}
		catch (...)
		{
			return false;
		}
		initDefaultOutputRT();
	
		SMatrix44 mt;
	
		mWorldTransposeMT = SMatrix44::identity();
		mViewTransposeMT = SMatrix44::identity();
		Math::buildOrthographicMatrixLH(&mt, (float)mViewPort[0].Width, (float)mViewPort[0].Height, 1.0f, 1000.0f);
		Math::matrixTranspose(&mOrthogTransposeMT, &mt);

		return initlaliseMaterial();
	}

	const SamplerState* CGuiRendererPass::getSamplerState() const
	{
		return mpMaterialIns->getMaterial()->getSampler();
	}

	bool CGuiRendererPass::initlaliseMaterial()
	{
		bool bValue = false;
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(szMaterName, Material));
		if (nullptr != pMaterial)
		{
			mpMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(szfontMaterName, Material));
			if (nullptr != pMaterial) 
			{
				bValue = true;
				mpFontMaterialIns = WL_NEW(CMaterialInstance, Instance)(pMaterial);
			}
		}

		return bValue;
	}

	void CGuiRendererPass::drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount)
	{
		size_t nIndex = 0;
		SRenderUnit* pRenderUnit = pRenderGroup->getRenderUnit();
		pRenderUnit->pMaterialInstance = mpMaterialIns;
		UINT16 nOrder = mpMaterialIns->getRenderOrder();
		int indexStep = 0;
		CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
		for (auto item : CGuiEngine::mShowGuiEntites)
		{
			if (item->mpUIWnd)
			{
				SGeometry geometry;
				geometry.pMaterialInstance = mpMaterialIns;
				geometry.pMatrix[WORLD_MT_0] = mWorldTransposeMT;
				geometry.pMatrix[VIEW_MT] = mViewTransposeMT;
				geometry.pMatrix[PROJECT_MT] = mOrthogTransposeMT;
				//geometry.pMatrix[VIEW_PROJECT_MT] = child.pMeshInstance->getTransposeViweProjectMatrix();

				geometry.indexType = FORMAT::FORMAT_R16_UINT;
				geometry.pVB = mpVBBuffer;
				geometry.pIB = mpIBBuffer;
				geometry.vertexOffset = 0;
				geometry.indexCount = 6;
				geometry.vertexSize = mVertexSize;
				
				geometry.pTextures = &item->mpUIWnd->mTextureMap;
				geometry.baseVertexLocation = 0;
				geometry.startIndex = indexStep * 6;
				++indexStep;

				pRenderUnit->geometry[nOrder].emplace_back(geometry);
							
				memcpy(&mVertics[nIndex], item->mpUIWnd->mData.mArea, sizeof(item->mpUIWnd->mData.mArea));
				nIndex += 4;
				std::map<TextureUseType, CTexture*> tempTextureMap;
				SGeometry* pGeometry = nullptr;
				for (auto child : item->mpUIWnd->mChildren)
				{					
					if (child->getWndType() != WndType::ui_label)
					{
						if (child->mTextureMap.size() == 0)
						{
							continue;
						}
						geometry.pMaterialInstance = mpMaterialIns;
						auto item = child->mTextureMap.find(TextureUseType::Base);
						if (item->second != nullptr)
						{
							if (tempTextureMap[TextureUseType::Base] != child->mTextureMap[TextureUseType::Base])
							{
								tempTextureMap = child->mTextureMap;
								geometry.pTextures = &child->mTextureMap;
								geometry.startIndex = indexStep * 6;
								geometry.indexCount = 6;
								pRenderUnit->geometry[nOrder].emplace_back(geometry);

								pGeometry = &pRenderUnit->geometry[nOrder].back();
							}
							else
							{
								pGeometry->indexCount += 6;
							}

							++indexStep;
							memcpy(&mVertics[nIndex], child->mData.mArea, sizeof(child->mData.mArea));
							nIndex += 4;
						}
					}
					else
					{
						geometry.startIndex = indexStep * 6;
						CUILabel* pLable = static_cast<CUILabel*>(child);
						auto nLenth = pLable->mszContent.length();
						if (nLenth > 0)
						{
							auto nSize = pLable->mDrawData.size();

							geometry.pTextures = &child->mTextureMap;
							geometry.pMaterialInstance = mpFontMaterialIns;

							geometry.indexCount = nLenth * 6;
							memcpy(&mVertics[nIndex], pLable->mDrawData.data(), sizeof(SVertexVTC) * nSize);
							nIndex += nSize;
							indexStep += nLenth;
						}
						pRenderUnit->geometry[nOrder].emplace_back(geometry);
					}
				}
			}
		}
		if (nIndex > 0)
		{
			mpVBBuffer->updateBuffer(pDeviceContext, mVertics.data(), sizeof(SVertexVTC) * nIndex);
		}		
	}


	void CGuiRendererPass::commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup)
	{
		for (auto& item : pRenderUnitGroup->mpRenderUnits)
		{
			for (auto _item : item->geometry)
			{
				auto pVB = mpVBBuffer->getBuffer();
				auto pIB = mpIBBuffer->getBuffer();
				for (auto& child : _item.second)
				{
					(child).pMaterialInstance->updateVSConstBuffer(0, (child).pMatrix->data());
					for (auto texture : *child.pTextures)
					{
						if (nullptr != texture.second)
						{
							(texture.second)->use(pDeviceContext, texture.first);
						}
					}
					(child).pMaterialInstance->commitToGpu(pDeviceContext);

					pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &(child).vertexSize, &(child).vertexOffset);
					// Set the index buffer to active in the input assembler so it can be rendered.
					pDeviceContext->IASetIndexBuffer(pIB, (DXGI_FORMAT)(child).indexType, 0);

					pDeviceContext->DrawIndexed((child).indexCount, child.startIndex, child.baseVertexLocation);
				}
			}
		}
	}

	void CGuiRendererPass::drawFinal()
	{
		CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
		if( CGuiEngine::mCacheGuiEntities.size() > 0 )
		{
			for (auto item : CGuiEngine::mCacheGuiEntities)
			{
				if (item.second)
				{
					CGuiEngine::mShowGuiEntites.push_back(item.first);
				}
				else
				{
					auto iter = std::find(CGuiEngine::mShowGuiEntites.begin(), CGuiEngine::mShowGuiEntites.end(), item.first);
					if (iter != CGuiEngine::mShowGuiEntites.end())
					{
						CGuiEngine::mShowGuiEntites.erase(iter);
					}
				}
			}
			CGuiEngine::mCacheGuiEntities.clear();
		}
	}

	constexpr int CGuiRendererPass::getVertexSize() const
	{
		return sizeof(SVertexVTC);
	}

}