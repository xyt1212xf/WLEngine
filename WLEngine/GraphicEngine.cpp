#include <sstream>
#include <algorithm>
#include "GraphicEngine.h"
#include "WLEngine.h"
#include "RenderStateMgr.h"
#include "MemoryMgr.h"
#include "Plane.h"
#include "Scene.h"
#include "GuiEntity.h"

#ifdef _WINDOWS
#include "Win.h"
#include "DX11Device.h"
#endif

namespace WL
{
	static int sRenderMaxEntities = 32;

	int iNumPerChunkRenderThreads = 0;
	std::vector<CommandList*>	CGraphicEngine::sCommandList;
	std::vector<HANDLE>			CGraphicEngine::sBeginRenderThreadSemaphore;
	std::vector<HANDLE>			CGraphicEngine::sEndRenderThreadEvent;
	std::vector<INT32>			CGraphicEngine::sRenderThreadEnd;
	std::vector<BYTE>			CGraphicEngine::sChunkQueue;
	std::vector<CActorEntity*>*	CGraphicEngine::sDrawEntities = nullptr;
	INT32						CGraphicEngine::sRenderThreadFlag = 0;

	CGraphicEngine* GGraphicPtr = nullptr;
	void CGraphicEngine::setWorldMatrix(Matrix44* pMT)
	{
		mWorldMatrix = *pMT;
	}

	void CGraphicEngine::setViewMatrxi(Matrix44* pMT)
	{
		mViewMatrix = *pMT;
	}

	void CGraphicEngine::setProjectMatrix(Matrix44* pMT)
	{
		mProjectMatrix = *pMT;
	}

	void CGraphicEngine::setOrthoMatrix(Matrix44* pMT)
	{
		mOrthoMatrix = *pMT;
	}

	void CGraphicEngine::setViewProjectMatrix(Matrix44* pViewProjectMT)
	{
		mViewProjectMatrix = *pViewProjectMT;
	}

	Matrix44* CGraphicEngine::getViewMatrix() 
	{
		return &mViewMatrix;
	}

	Matrix44* CGraphicEngine::getProjectMatrix() 
	{
		return &mProjectMatrix;
	}

	Matrix44* CGraphicEngine::getOrthoMatrix()
	{
		return &mOrthoMatrix;
	}

	Matrix44* CGraphicEngine::getWorldMatrix()
	{
		return &mWorldMatrix;
	}

	
	Matrix44* CGraphicEngine::getViweProjectMatrix()
	{
		return &mViewProjectMatrix;
	}

	void CGraphicEngine::setFillCullMode(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		mpDevice->mpRenderStateMgr->setFillCullMode(pContext, pRS);
	}

	void CGraphicEngine::setAlpha(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		mpDevice->mpRenderStateMgr->setAlpha(pContext, pRS);
	}

	void CGraphicEngine::setDepthStencil(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		mpDevice->mpRenderStateMgr->setDepthStencil(pContext, pRS);
	}

	void CGraphicEngine::setDepth(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		mpDevice->mpRenderStateMgr->setDepth(pContext, pRS);
	}

	void CGraphicEngine::setDepthCompare(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		mpDevice->mpRenderStateMgr->setDepthCompare(pContext, pRS);
	}


	void CGraphicEngine::setRenderState(DeviceContext* pContext, RenderStateDesc* pRS)
	{
		setFillCullMode(pContext, pRS);
		setAlpha(pContext, pRS);
		setDepthStencil(pContext, pRS);
		setDepth(pContext, pRS);
		setDepthCompare(pContext, pRS);
	}

	void CGraphicEngine::setDepthStencilState(DeviceContext* pContext, RenderState* pRS, int nStencilRef/*= 1*/)
	{
		mpDevice->mpRenderStateMgr->setDepthStencilState(pContext, pRS, nStencilRef);
	}

	void CGraphicEngine::setBlendState(DeviceContext* pContext, RenderState* pRS)
	{
		mpDevice->mpRenderStateMgr->setBlendState(pContext, pRS);
	}

	void CGraphicEngine::setRasterizerState(DeviceContext* pContext, RenderState* pRS)
	{
		mpDevice->mpRenderStateMgr->setRasterizerState(pContext, pRS);
	}

	void CGraphicEngine::setTransposeWorldMatrix(Matrix44* pMT)
	{
		mTransposeWorldMatrix = *pMT;
	}

	void CGraphicEngine::setTransposeViewMatrxi(Matrix44* pMT)
	{
		mTransposeViewMatrix = *pMT;
	}

	void CGraphicEngine::setTransposeProjectMatrix(Matrix44* pMT)
	{
		mTransposeProjectMatrix = *pMT;
	}

	void CGraphicEngine::setTransposeOrthoMatrix(Matrix44* pMT)
	{
		mTransposeOrthoMatrix = *pMT;
	}

	void CGraphicEngine::setTransposeViewProjectMatrix(Matrix44* pViewProjectMT)
	{
		mTransposeViewProjectMatrix = *pViewProjectMT;
	}

	const std::list<WL::shaderParam>& CGraphicEngine::getGlobalShaderParam(ShaderBufferType type)
	{
		auto iter = mConstBufferParamMap.find(type);
		if (iter != mConstBufferParamMap.end())
		{
			return iter->second;
		}
		else
		{
			return *(std::list<WL::shaderParam>*)(0);
		}
	}

	void CGraphicEngine::addGlobalShaderParam(ShaderBufferType type, const shaderParam& param)
	{
		mConstBufferParamMap[type].emplace_back(param);
	}

	Matrix44* CGraphicEngine::getTransposeWorldMatrix()
	{
		return &mTransposeWorldMatrix;
	}

	Matrix44* CGraphicEngine::getTransposeViewMatrix()
	{
		return &mTransposeViewMatrix;
	}

	Matrix44* CGraphicEngine::getTransposeProjectMatrix()
	{
		return &mTransposeProjectMatrix;
	}

	Matrix44* CGraphicEngine::getTransposeOrthoMatrix()
	{
		return &mTransposeOrthoMatrix;
	}

	Matrix44* CGraphicEngine::getTransposeViweProjectMatrix()
	{
		return &mTransposeViewProjectMatrix;
	}

	void CGraphicEngine::addEntity(CEntity* pEntitiy)
	{
		auto iter = std::find(mAllEntities.begin(), mAllEntities.end(), pEntitiy);
		if (mAllEntities.end() == iter)
		{
			mAllEntities.emplace_back(pEntitiy);
			pEntitiy->addRef();
		}
	}

	void CGraphicEngine::removeEntity(CEntity* pEntitiy)
	{
		auto iter = std::find(mAllEntities.begin(), mAllEntities.end(), pEntitiy);
		if (mAllEntities.end() != iter)
		{
			WL_DECREASE((*iter));
			mAllEntities.erase(iter);
		}
	}

	CDevice* CGraphicEngine::getDevice()
	{
		return mpDevice;
	}

	GFXDevice* CGraphicEngine::getGraphicsDevice()
	{
		return nullptr != mpDevice ? mpDevice->getGraphicsDevice() : nullptr;
	}

	DeviceContext* CGraphicEngine::getImmediateContext()
	{
#ifdef _DEBUG
		assert(mpImmediateContext != nullptr);
#endif // DEBUG
		return mpImmediateContext;
	}

	DeviceContext* CGraphicEngine::getDeviceContext(int nIndex)
	{
#ifdef _DEBUG
		assert(nIndex < (int)mpDeviceContext.size());
#endif // DEBUG

		return mpDeviceContext[nIndex];
	}


	std::vector<CRenderer*>& CGraphicEngine::getRendererGroup() const
	{
		return mpRendererArray;
	}

	void CGraphicEngine::createRendererGroup()
	{
		CRenderer* pRender = nullptr;
	//	pRender = new CShadowRenderer();
	//	mpRendererArray.emplace_back(pRender);
		pRender = WL_NEW(CFrontRenderer, Renderer);
		mpRendererArray.emplace_back(pRender);
		pRender = WL_NEW(CDeferredRenderer, Renderer);
		mpRendererArray.emplace_back(pRender);
		//pRender = new CTransparentRenderer();
		//mpRendererArray.emplace_back(pRender);
		//pRender = WL_NEW(CGuiRenderer, Renderer);
		//mpRendererArray.emplace_back(pRender);

		//enableRenderer<CShadowRenderer>();
		//enableRenderer<CDeferredRenderer>();
		//enableRenderer<CTransparentRenderer>();
		//disableDeferredRenderer();
		enableDeferredRenderer();
	}

	void CGraphicEngine::clearDeviceContext(bool bImmediateContext /*= false*/)
	{
		for( auto item : mpDeviceContext)
		{
			item->ClearState();
		}
		if (bImmediateContext)
		{
			mpImmediateContext->ClearState();
		}
	}

	CRenderer* CGraphicEngine::getCurrentRendererPtr() const
	{
		return mpCurrentRenderer;
	}

	CRendererPass* CGraphicEngine::getCurrentRendererPassPtr() const
	{
		return mpCurrentRendererPass;
	}

	CSurfaceView* CGraphicEngine::createRendererSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags)
	{
		auto pRenderSurface = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
		if (nullptr != pRenderSurface)
		{
			if (!pRenderSurface->createRenderSurface(nWidth, nHeight, nTextureFormat, depthFormat, bindInfo, miscFlags))
			{
				WL_DECREASE(pRenderSurface);
				pRenderSurface = nullptr;
			}
		}
		return pRenderSurface;
	}

	CSurfaceView* CGraphicEngine::createRenderTarget(RenderTargetType nFlag, int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags)
	{
		CSurfaceView* pSurface = nullptr;
		auto iter = mRTArray.find(nFlag);
		if (iter == mRTArray.end())
		{
			pSurface = createRendererSurface(nWidth, nHeight, nTextureFormat, depthFormat, bindInfo, miscFlags);
			if (nullptr != pSurface)
			{
				mRTArray[nFlag] = pSurface;
			}
		}
		else
		{
			pSurface = iter->second;
		}
		return pSurface;
	}

	CSurfaceView* CGraphicEngine::getRenderTarget(RenderTargetType nFlag)
	{
		CSurfaceView* pRT = nullptr;
		auto iter = mRTArray.find(nFlag);
		if (iter != mRTArray.end())
		{
			pRT = iter->second;
		}
		return pRT;
	}

	void CGraphicEngine::destoryRenderTarget(RenderTargetType nFlag)
	{
		auto iter = mRTArray.find(nFlag);
		if (iter != mRTArray.end())
		{
			WL_DECREASE(iter->second);
			mRTArray.erase(iter);
		}
	}

	bool CGraphicEngine::setRenderTarget(RenderTargetType nFlag, DeviceContext* pContext)
	{
		auto iter = mRTArray.find(nFlag);
		if (iter != mRTArray.end())
		{
			(iter->second)->setRenderSurface(pContext);
			return true;
		}
		return false;
	}

	bool CGraphicEngine::setRenderTarget(CSurfaceView* pRTT, DeviceContext* pContext)
	{
		if (nullptr != pRTT)
		{
			pRTT->setRenderSurface(pContext);
			return true;
		}
		return false;
	}

	void CGraphicEngine::storeRenderTarget(RenderTargetType nFlag, CSurfaceView* pRTT)
	{
		if (nullptr != pRTT)
		{
			pRTT->addRef();
			auto iter = mRTArray.find(nFlag);
			if (iter != mRTArray.end())
			{
				WL_DECREASE(iter->second);
			}
			mRTArray[nFlag] = pRTT;
		}
	}

	void CGraphicEngine::clearRenderTarget(RenderTargetType nFlag)
	{
		auto iter = mRTArray.find(nFlag);
		if (iter != mRTArray.end())
		{
			(iter->second)->clearRenderSurface(mpImmediateContext);
		}
	}

	void CGraphicEngine::clearRenderTarget(CSurfaceView* pRTT)
	{
		if (nullptr != pRTT)
		{
			pRTT->clearRenderSurface(mpImmediateContext);
		}
	}

	void CGraphicEngine::enableDeferredRenderer()
	{
		disableRenderer<CFrontRenderer>();
		enableRenderer<CDeferredRenderer>();
	}

	void CGraphicEngine::disableDeferredRenderer()
	{
		disableRenderer<CDeferredRenderer>();
		enableRenderer<CFrontRenderer>();
	}

	bool CGraphicEngine::restoreDeviceRenderTargets()
	{
		return mpDevice->restoreRenderTargets();
	}

	void CGraphicEngine::deleteRenderer(int nOrder)
	{
		disableRenderer(nOrder);
		auto iter = mpRendererArray.begin();
		while (iter != mpRendererArray.end())
		{
			auto pRenderer = (*iter);
			if (pRenderer->getOrder() == nOrder)
			{
				mpRendererArray.erase(iter);
				break;
			}
			++iter;
		}
	}

	void CGraphicEngine::disableRenderer(int nOrder)
	{
		auto iter = mpActiveRenderer.begin();
		while (iter != mpActiveRenderer.end())
		{
			auto pRenderer = (iter->second);
			if (pRenderer->getOrder() == nOrder)
			{
				pRenderer->mEnable = false;
				mpActiveRenderer.erase(iter);
				break;
			}
			++iter;
		}
	}

	void CGraphicEngine::procesCommandBuf()
	{
		//mCommandBufIndex = mCommandBufIndex & 1;
		if (mCommandBuf.size() > 0)
		{
			lockCommandBuf();
			for (auto item : mCommandBuf)
			{
				switch (item.nTyep)
				{
				case commandType::ShotScreen:
				{

				}
				break;
				case commandType::StopDraw:
				{
					mbDrawed = false;
				}
				break;
				case commandType::RecoverDraw:
				{
					mbDrawed = true;
				}
				break;
				default:
					break;
				}
			}
			mCommandBuf.clear();
			unLockCommandBuf();
		}
		//++mCommandBufIndex;
	}

	//void CGraphicEngine::addRendererPass(CRendererPass* pRenderer)
	//{
	//	if (nullptr != pRenderer)
	//	{
	//		auto iter = std::find(mArrayRenderer.begin(), mArrayRenderer.end(), pRenderer);
	//		if (mArrayRenderer.end() == iter)
	//		{
	//			pRenderer->addRef();
	//			mArrayRenderer.emplace_back(pRenderer);
	//			mArrayRenderer.sort(RendererPassCompare());
	//		}
	//	}
	//}

	//void CGraphicEngine::removeRendererPass(CRendererPass* pRenderer)
	//{
	//	if (nullptr != pRenderer)
	//	{
	//		auto iter = std::find(mArrayRenderer.begin(), mArrayRenderer.end(), pRenderer);
	//		if (mArrayRenderer.end() != iter)
	//		{
	//			mArrayRenderer.erase(iter);
	//			SafeRelease((*iter));
	//		}
	//	}
	//}

	void CGraphicEngine::rendererThread(int nIndex)
	{
		std::stringstream szThreadName;
		szThreadName << "rendererThread_" << nIndex;
		CThreadPool::setThreadName(szThreadName.str().c_str() , GetCurrentThreadId());
		DeviceContext* pDeviceContext = GEngine->getDeviceContext(nIndex);

		while (true)
		{
			WaitForSingleObject(sBeginRenderThreadSemaphore[nIndex], INFINITE);
			if (!GEngine->isRun())
			{
				break;
			}
	
			auto pRenderPass = GEngine->getCurrentRendererPassPtr();
			if (pRenderPass->drawBegin(pDeviceContext, true))
			{
				auto pRender = GEngine->getCurrentRendererPtr();
				int nBegin = nIndex * sRenderMaxEntities;
				int nSize = (int)sDrawEntities->size() - (nIndex)*sRenderMaxEntities;
				nSize = nSize > sRenderMaxEntities ? sRenderMaxEntities : nSize;
				auto pRenderUnitGroup = pRender->getRenderUnitGrounp(nIndex);
				pRenderPass->drawEntity(pDeviceContext, pRenderUnitGroup, (*sDrawEntities), nBegin, nSize);
				pRenderPass->drawEnd(pDeviceContext, pRenderUnitGroup, sCommandList[nIndex], nIndex);
			}
			SetEvent(sEndRenderThreadEvent[nIndex]);
		}
		pDeviceContext = nullptr;
		SetEvent(sEndRenderThreadEvent[nIndex]);
		sRenderThreadEnd[nIndex] = 1;
	}

	CGraphicEngine::CGraphicEngine()
	{
		GGraphicPtr = this;
	}

	CGraphicEngine::~CGraphicEngine()
	{

	}

	bool CGraphicEngine::initialize()
	{
		mpDevice = WL_NEW(CDevice, GfxDevice);
		mpDevice->initDevice();
		mpImmediateContext = mpDevice->getImmediateContext();
		GEngine->threadDetach([]()
			{
				CThreadPool::setThreadName("renderFrameThread", GetCurrentThreadId());
				GEngine->renderFrame();				
			});

		iNumPerChunkRenderThreads = std::thread::hardware_concurrency() / 2;
		sBeginRenderThreadSemaphore.resize(iNumPerChunkRenderThreads);
		sEndRenderThreadEvent.resize(iNumPerChunkRenderThreads);
		sRenderThreadEnd.resize(iNumPerChunkRenderThreads);
		sChunkQueue.resize(iNumPerChunkRenderThreads);
		createRendererGroup();
		
		int i = 0;
		while (true)
		{
			auto pDeviceContext = mpDevice->getDeviceContext(i++);
			if (nullptr != pDeviceContext)
			{
				mpDeviceContext.push_back(pDeviceContext);
				sCommandList.push_back(nullptr);
			}
			else
			{
				break;
			}
		}
		static std::vector<int> nIndex(iNumPerChunkRenderThreads);
		for (int nCount = 0; nCount < iNumPerChunkRenderThreads; ++nCount)
		{
			nIndex[nCount] = nCount;
			sRenderThreadEnd[nCount] = 0;
			sBeginRenderThreadSemaphore[nCount] = CreateSemaphore(NULL, 0, 1024, NULL);
			sEndRenderThreadEvent[nCount] = CreateEvent(NULL, FALSE, FALSE, NULL);
			GEngine->threadDetach(CGraphicEngine::rendererThread, nIndex[nCount]);
		}

		return true;
	}

	void CGraphicEngine::unInitialize()
	{
		for (int i = 0; i < iNumPerChunkRenderThreads; ++i)
		{
			ReleaseSemaphore(sBeginRenderThreadSemaphore[i], 1, nullptr);
		}
		_waitRenderThreadEnd();
		for (int iInstance = 0; iInstance < iNumPerChunkRenderThreads; ++iInstance)
		{
			CloseHandle(sBeginRenderThreadSemaphore[iInstance]);
			CloseHandle(sEndRenderThreadEvent[iInstance]);
		}
		WL_DECREASE(mpDevice);
		for (auto item : mpRendererArray)
		{
			//SafeDelete(item);
			WL_DELETE(item, Renderer)
		}
		for (auto item : mAllEntities)
		{
			WL_DECREASE(item);
		}
		mAllEntities.clear();
		mpRendererArray.clear();
		GGraphicPtr = nullptr;
	}

	void CGraphicEngine::renderFrame()
	{
		static DWORD nTime = GetTickCount();
		while (GEngine->isRun())
		{
#if 0
			static CTexture  c;

			CTexture* a = dynamic_cast<CTexture*>(GEngine->createResource("resource/Texture/1.png", Texture));
			CTexture* b = dynamic_cast<CTexture*>(GEngine->createResource("resource/Texture/grass_top.png", Texture));
			
			auto pDevice = Dev->getImmediateContext();
			pDevice->CSSetShader(CS, nullptr, 0);
			auto ap = a->getShaderResView();
			auto bp = b->getShaderResView();
			pDevice->CSSetShaderResources(0, 1, &ap);
			pDevice->CSSetShaderResources(1, 1, &bp);
			c.initialiseUnorderedTexture(128, 128, GI_FORMAT_R8G8B8A8_UNORM, BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS, UAV_DIMENSION_TEXTURE2D);

			auto pc = c.getUnordAccessView();
			pDevice->CSSetUnorderedAccessViews(0, 1, &pc, nullptr);
			pDevice->Dispatch(32, 32, 1);
			SAFE_RELEASE(ap);
			SAFE_RELEASE(ab);
			//	SaveDDSTextureToFile(m_pd3dImmediateContext.Get(), m_pTextureOutput.Get(), L"..\\Texture\\flareoutput.dds");
			c.saveToFile("c:/WLEngine/1.png");
#endif
			procesCommandBuf();
			if (mbDrawed)
			{
				GEngine->renderWorld();
			}
#if 0 
			INT32 nRunTime = GetTickCount();
			if (nRunTime - nTime < 15)
			{
				Sleep(15 - (nRunTime - nTime) );
			}
			nTime = nRunTime;
#endif
		}
		do
		{
			int i = 0;
			for (auto item : sRenderThreadEnd)
			{
				if (item == 1)
				{
					++i;
				}
			}
			if (i == iNumPerChunkRenderThreads)
			{
				break;
			}
			Sleep(50);
		} while (true);
		sRenderThreadFlag = 1;
	}

	void CGraphicEngine::_update(UINT32 dTime)
	{

	}

	void CGraphicEngine::_waitRenderThreadEnd()
	{
		do
		{
			if (1 == sRenderThreadFlag)
			{
				break;
			}
			Sleep(100);
		} while (true);
	}

	void CGraphicEngine::drawEntity(std::vector<CActorEntity*>& pDrawEntitiesArray, UINT32 dTime)
	{
		static UINT32 nNowTime = GetTickCount();
		static int nFrameCount = 0;

		CGuiEntity* pGuiEntity = nullptr;

		auto pConfig = GEngine->getWindowConfigPtr();
		if (pConfig->bShowFps)
		{
			auto pScene = GEngine->getCurrentScenePtr();
			if (nullptr != pScene)
			{
				auto pGuis = pScene->getGuis();
				if (pGuis.size() > 0)
				{
					for (auto gui : pGuis)
					{
						if (gui->getEntityName() == "FPS")
						{
							pGuiEntity = gui;
						}
					}
				}
			}
		}
		GEngine->copyLightData();
		int nOldRenderSize = sRenderMaxEntities;
		mpDevice->beginDrawScene();

		auto iter = mpActiveRenderer.begin();
		auto iterEnd = mpActiveRenderer.end();
		while (iter != iterEnd)
		{
			mpCurrentRenderer = iter->second;
			mpCurrentRenderer->begin();

			for (auto pass : (mpCurrentRenderer)->mListRenderPass)
			{
				mpCurrentRendererPass = pass;
				sDrawEntities = &(pass->checkEntities(pDrawEntitiesArray));
				int nSize = (int)sDrawEntities->size();
				int nStartRenderStartCount = (nSize - 1) / sRenderMaxEntities + 1;
				nStartRenderStartCount = nStartRenderStartCount > iNumPerChunkRenderThreads ? iNumPerChunkRenderThreads : nStartRenderStartCount;
				if (nStartRenderStartCount * sRenderMaxEntities < nSize)
				{
					sRenderMaxEntities = int((float)nSize / (float)iNumPerChunkRenderThreads + 0.5f);
				}
				for (int i = 0; i < nStartRenderStartCount; ++i)
				{
					ReleaseSemaphore(sBeginRenderThreadSemaphore[i], 1, nullptr);
				}
				if (GEngine->isRun())
				{
					WaitForMultipleObjects(nStartRenderStartCount, sEndRenderThreadEvent.data(), TRUE, INFINITE);
					for (int i = 0; i < nStartRenderStartCount; ++i)
					{
						auto pCommandList = sCommandList[i];
						if (nullptr != pCommandList)
						{
							GEngine->getImmediateContext()->ExecuteCommandList(pCommandList, FALSE);
							DEV_RELEASE(sCommandList[i]);
						}
					}
					GEngine->restoreDeviceRenderTargets();
				}
				else
				{
					iter = mpActiveRenderer.end();
					--iter;
					break;
				}
			}
			mpCurrentRenderer->end();
			++iter;
		}
		mpDevice->endDrawScene();
		sRenderMaxEntities = nOldRenderSize;

		++nFrameCount;
		if (pGuiEntity)
		{
			DWORD tt = GetTickCount();
			if( tt - nNowTime >= 1000 )
			{
				nNowTime = tt;
				std::string fps = "Fps:" + std::to_string(nFrameCount);
				pGuiEntity->setText(1, fps);
				nFrameCount = 0;
			}
		}
	}

}
