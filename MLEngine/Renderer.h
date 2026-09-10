#pragma once
#include "RenderChunk.h"
#include "Entity.h"
#include "LightEntity.h"
#include "CameraEntity.h"
#include "LightMeshRendererPass.h"
#include "DeferredLightRendererPass.h"
#include "DepthRendererPass.h"
#include "DeferredRendererPass.h"
#include "NoLightRendererPass.h"
#include "ShadowMapRendererPass.h"
#include "GuiRendererPass.h"

namespace WL
{
	enum  RendererType
	{
		Renderer = 0,
		CustomRenderer = 99,
		ShadowRenderer = 100,
		FrontRenderer = 200,
		DeferredRenderer = 300,
		GuiRenderer = 400,
	};
	
	enum  DrawQualityLevel
	{
		Low,
		Middle,
		Hihg,
	};


	class CRenderer
	{
		friend class CGraphicEngine;
	public:
		CRenderer();
		CRenderer(int nOrder );
		virtual ~CRenderer();
		SRenderUnitGrounp* getRenderUnitGrounp(INT32 nIndex);
		int getOrder();
		virtual void drawEntity(SRenderUnitGrounp* pRenderGroup, std::vector<CActorEntity*>& entities, int nBegin, int nCount);
		virtual bool drawBegin(DeviceContext* pDeviceContext, bool bCleanState);
		virtual void drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext);
		virtual void begin();
		virtual void end();
		void setQualityLevel(DrawQualityLevel nLevle);
		DrawQualityLevel getQualityLevel()const;
		void addRendererPass(CRendererPass* pPass);
		void removeRendererPass(CRendererPass* pPass);
		
		template<typename T>
		T* findRendererPass()
		{
			T* pRendererPass = nullptr;
			for (auto item : mListRenderPass)
			{
				pRendererPass = dynamic_cast<T*>(item);
				if (pRendererPass != nullptr)
				{
					pRendererPass = item;
					break;
				}
			}
			return pRendererPass;
		}
		
#ifdef _DEBUG
		void setClearColor(const Vec4F& color);
#endif
	protected:
		void commitToGpu(DeviceContext* pDeviceContext, std::list<SRenderUnit*>& renderChunk);
		virtual std::vector<CActorEntity*>& checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray);

	protected:
		bool mEnable = false;
		int mOrder = Renderer;
		DrawQualityLevel mDrawQualityLevel = Middle;
		const static int sMaxRenderChunk = 32;
		SRenderUnitGrounp mRenderChunkGroup[sMaxRenderChunk];
		std::vector<CActorEntity*>	mDrawEntities;
		std::map<int, std::list<CRendererPass*>>	mArrayRendererPass;
		Vec4F mBackColor = { 0, 0, 0, 1 };
		std::list<CRendererPass*> mListRenderPass;
	};
}