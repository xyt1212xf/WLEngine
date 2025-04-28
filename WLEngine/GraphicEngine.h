#pragma once
#include "Device.h"
#include "Matrix.h"
#include "GuiRenderer.h"
#include "FrontRenderer.h"
#include "DeferredRenderer.h"
#include "EngineBase.h"
#include "RendererPass.h"
#include "DefMemory.h"

namespace WL
{
	static const Vec4F BLACK = { 0,0,0,1 };
	static const Vec4F WHITE = { 1,1,1,1 };
	static std::map<CustomKeyWorld, std::string > globalKeyWorld =
	{
		{CustomKeyWorld::GlobalLight, "GlobalLight"},
		{CustomKeyWorld::AmbientLight,"AmbientLight"},
	};
	class CSurfaceView;
	class CGraphicEngine : virtual public CEngineBase
	{
	public:
		CGraphicEngine();
		virtual ~CGraphicEngine();
		bool initialize();
		void unInitialize();
		void renderFrame();

		void drawEntity(std::vector<CActorEntity*>& pDrawEntitiesArray, UINT32 dTime);

		void setWorldMatrix(Matrix44* pMT);
		void setViewMatrxi(Matrix44* pMT);
		void setProjectMatrix(Matrix44* pMT);
		void setOrthoMatrix(Matrix44* pMT);
		void setViewProjectMatrix(Matrix44* pViewProjectMT);
		Matrix44* getWorldMatrix();
		Matrix44* getViewMatrix();
		Matrix44* getProjectMatrix();
		Matrix44* getOrthoMatrix();
		Matrix44* getViweProjectMatrix();
		void setFillCullMode(DeviceContext* pContext, RenderStateDesc* pRS);
		void setAlpha(DeviceContext* pContext, RenderStateDesc* pRS);
		void setDepthStencil(DeviceContext* pContext, RenderStateDesc* pRS);
		void setDepth(DeviceContext* pContext, RenderStateDesc* pRS);
		void setDepthCompare(DeviceContext* pContext, RenderStateDesc* pRS);
		void setRenderState(DeviceContext* pContext, RenderStateDesc* pRS);
		void setDepthStencilState(DeviceContext* pContext, RenderState* pRS, int nStencilRef = 0);
		void setBlendState(DeviceContext* pContext, RenderState* pRS);
		void setRasterizerState(DeviceContext* pContext, RenderState* pRS);

		void setTransposeWorldMatrix(Matrix44* pMT);
		void setTransposeViewMatrxi(Matrix44* pMT);
		void setTransposeProjectMatrix(Matrix44* pMT);	
		void setTransposeOrthoMatrix(Matrix44* pMT);
		void setTransposeViewProjectMatrix(Matrix44* pViewProjectMT);

		const std::list<shaderParam>& getGlobalShaderParam(ShaderBufferType type);
		void addGlobalShaderParam(ShaderBufferType type, const shaderParam& param);
		Matrix44* getTransposeWorldMatrix();
		Matrix44* getTransposeViewMatrix();
		Matrix44* getTransposeProjectMatrix();
		Matrix44* getTransposeOrthoMatrix();
		Matrix44* getTransposeViweProjectMatrix();
		
		void addEntity(CEntity* pEntitiy);
		void removeEntity(CEntity* pEntitiy);
		CDevice* getDevice();
		GFXDevice* getGraphicsDevice();
		DeviceContext* getImmediateContext();
		DeviceContext* getDeviceContext(int nIndex);
		std::vector<CRenderer*>& getRendererGroup()const;
		void createRendererGroup();
		void clearDeviceContext(bool bImmediateContext = false);
		CRenderer* getCurrentRendererPtr()const;
		CRendererPass* getCurrentRendererPassPtr()const;
		CSurfaceView* createRendererSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags);
		CSurfaceView* createRenderTarget(RenderTargetType nFlag, int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags);
		CSurfaceView* getRenderTarget(RenderTargetType nFlag);
		void destoryRenderTarget(RenderTargetType nFlag);
		bool setRenderTarget(RenderTargetType nFlag, DeviceContext* pContext);
		bool setRenderTarget(CSurfaceView* pRTT, DeviceContext* pContext);
		void storeRenderTarget(RenderTargetType nFlag, CSurfaceView* pRTT);

		void clearRenderTarget(RenderTargetType nFlag);
		void clearRenderTarget(CSurfaceView* pRTT);
		void enableDeferredRenderer();
		void disableDeferredRenderer();
		bool restoreDeviceRenderTargets();

		template<typename T>
		bool createRenderer(int nOrder);

		void deleteRenderer(int nOrder);

		template<typename T>
		const T* getRenderer()const;


		template<typename T>
		void enableRenderer();

		template<typename T>
		void disableRenderer();

		void disableRenderer(int nOrder);

		void procesCommandBuf();
		//void addRendererPass(CRendererPass* pRenderer);
		//void removeRendererPass(CRendererPass* pRenderer);

	protected:
		static void rendererThread(int nIndex);
		virtual void _update(UINT32 dTime);
		void _waitRenderThreadEnd();

	protected:
		static std::vector<CommandList*>	sCommandList;
		static std::vector<HANDLE>			sBeginRenderThreadSemaphore;
		static std::vector<HANDLE>			sEndRenderThreadEvent;
		static std::vector<BYTE>			sChunkQueue;
		static std::vector<INT32>			sRenderThreadEnd;
		static std::vector<CActorEntity*>*	sDrawEntities;
		
		static INT32 sRenderThreadFlag;

		std::list<CEntity*>	mAllEntities;
		CDevice*	mpDevice = nullptr;
		
		Matrix44	mWorldMatrix;
		Matrix44	mViewMatrix;
		Matrix44	mProjectMatrix;
		Matrix44	mOrthoMatrix;
		Matrix44	mViewProjectMatrix;

		Matrix44 mTransposeWorldMatrix;
		Matrix44 mTransposeViewMatrix;
		Matrix44 mTransposeProjectMatrix;
		Matrix44 mTransposeOrthoMatrix;
		Matrix44 mTransposeViewProjectMatrix;

		DeviceContext* mpImmediateContext = nullptr;
		std::vector<DeviceContext*>		mpDeviceContext;
		CRenderer* mpCurrentRenderer = nullptr;
		CRendererPass* mpCurrentRendererPass = nullptr;
		mutable std::vector<CRenderer*>	mpRendererArray;
		std::map<INT32, CRenderer*>	mpActiveRenderer;
		std::map<ShaderBufferType, std::list<shaderParam>> mConstBufferParamMap;
		std::map<RenderTargetType, CSurfaceView*>	mRTArray;
		
		bool  mbDrawed = false;
	//	std::list<CRendererPass*>	mArrayRenderer;
	};
	extern CGraphicEngine* GGraphicPtr;

	template<typename T>
	bool CGraphicEngine::createRenderer(int nOrder)
	{
		auto pRender = WL_NEW(T, Renderer)(nOrder);
		if (nullptr != pRender)
		{
			mpRendererArray.emplace_back(pRender);
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	const T* CGraphicEngine::getRenderer() const
	{
		T* pRenderer = nullptr;
		for (auto item : mpRendererArray)
		{
			pRenderer = dynamic_cast<T*>(item);
			if (nullptr != pRenderer)
			{
				break;
			}
		}
		return pRenderer;
	}

	template<typename T>
	void CGraphicEngine::enableRenderer()
	{
		for (auto item : mpRendererArray)
		{
			auto pRenderer = dynamic_cast<T*>(item);
			if (nullptr != pRenderer)
			{
				pRenderer->mEnable = true;
				int nOrder = pRenderer->getOrder();
				mpActiveRenderer[nOrder] = item;
				break;
			}
		}
	}


	template<typename T>
	void CGraphicEngine::disableRenderer()
	{
		auto iter = mpActiveRenderer.begin();
		while (iter != mpActiveRenderer.end())
		{
			auto pRenderer = dynamic_cast<T*>(iter->second);
			if (nullptr != pRenderer)
			{
				pRenderer->mEnable = false;

				mpActiveRenderer.erase(iter);
				break;
			}
			++iter;
		}
	}


}

