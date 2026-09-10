#pragma once
#include "MaterialInstance.h"
#include "ShaderInstance.h"
#include "RenderChunk.h"
#include "Refcount.h"


namespace WL
{
	class CRenderer;
	class CSurfaceView;
	enum class eRendererPass
	{
		InvalidPass = 0,
		ActorDrawPass = 1,
		LightMeshPass = 100,
		DepthPass = 200,
		DeferredLightPass = 300,
		ShadowPass = 400,
		NoLightActorPass = 500,
		GuiPass = 1000,
	};

	enum class RenderTargetType
	{
		OutputRTT_0 = 0,
		OutputRTT_1,
		OutputRTT_2,
		OutputRTT_3,
		OutputRTT_4,
		OutputRTT_5,
		OutputRTT_6,
		OutputRTT_7,
		PositionRTT = 8,
		ColorRTT = 9,
		NormalRTT = 10,
		LightRTT = 11,
		DepthRTT = 12,
		DeferredRTT = 13,
		CustomRTTBegin = 30,
	};

	//////////////////////////////////////////////////////////////////////////
	class CRendererPass : public CRefcount
	{
	public:
		struct SRendererPassCompare
		{
			bool operator()(CRendererPass* a, CRendererPass* b)
			{
				return a->mOrder < b->mOrder;
			}
		};
		friend class CGraphicEngine;
		friend class CRenderer;
		friend struct SRendererPassCompare;

	public:
		CRendererPass(CRenderer* pRenderer);
		virtual ~CRendererPass();
		virtual bool initialise();
		virtual void clearSurface();
		virtual void setInputRT(std::vector<CSurfaceView*>& pRenderSurface);
		void addInputRT(CSurfaceView* pRenderSurface);
		bool operator<(const CRendererPass* r);
		UINT32 getOrder()const;

		std::vector<CSurfaceView*>& getArrayOutputRT();
		CSurfaceView* getOutputRT(int nIndex);
		

	protected:
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEntity(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount);
		virtual void drawEnd(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderGroup, CommandList*& pCommandList, int nContext);

		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray);
		virtual void commitToGpu(DeviceContext* pDeviceContext, SRenderUnitGrounp* pRenderUnitGroup);
		virtual bool initlaliseMaterial();
		virtual void initlaliseOutputRT();
		virtual void drawFinal();

		void initDefaultOutputRT();
		void drawGeometry(SRenderUnit* pItem, DeviceContext* pDeviceContext);
			
	protected:
		int mOutputBufferSize = 0;
		CRenderer* mpRenderer = nullptr;
		Vec4F mClearColor = { 0, 0, 0, 1 };
		std::vector<CSurfaceView*> mArrayOutputRTT;
		std::vector<CSurfaceView*> mArrayInputRTT;
		std::vector<RenderTargetView*> mArrayOutputRTTView;

		//除非不是Graphic，否者不需要RendererPass自己清除
		DepthStencilView* mpDepthStencilView = nullptr;

		std::vector<CActorEntity*>	mDrawEntities;
		CMaterialInstance* mpMaterialIns = nullptr;
		eRendererPass mOrder = eRendererPass::InvalidPass;
		std::vector<SViewPort> mViewPort;
		CMutex mMutex;
	};


}
