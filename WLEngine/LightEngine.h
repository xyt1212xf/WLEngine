#pragma once
#include "LightEntity.h"
#include "EngineBase.h"

namespace WL
{
	class CLightEngine : virtual public CEngineBase
	{
		friend class CResourceMgr;
	public:
		CLightEngine();
		virtual ~CLightEngine();
		void enableOriginMT();
		void disableOriginMT();
		void addLightEntity(CLightEntity* pLight);
		void removeLight(CLightEntity* pLight);
		bool initialize();
		void unInitialize();
		void setAllLights(CMaterialInstance* pMaterialInstance);
				
		void setEnvironmentLight(CMaterialInstance* pMaterialInstance);
		void setLightMatrix(CMaterialInstance* pMaterialInstance);
		void setDynamicLight(CMaterialInstance* pMaterialInstance);
		INT32 getLightCount();
		INT32 getLightCountTest();
		void copyLightData();
		void setAmbientLight(const Vec4F& color);
		const Vec4F& getAmbientLight()const;
		void setDiffuseLight(const Vec4F& color);
		const Vec4F& getDiffuseLight()const;
		void setDiffuseDirection(const Vec4F& dir);
		const Vec4F& getDiffuseDirection()const;
		
		SMatrix44& getLightViewMT();
		SMatrix44& getLightTransposeViewMT();

		SMatrix44& getLightOriginMT();
		SMatrix44& getLightProjectionMT();

		SMatrix44& getLightTransposeProjectionMT();

		SMatrix44& getLightTransposeVPMT();
	protected:
		virtual void _update(UINT32 dTime);
		
	private:
		Vec4F	mAmbientLight = {0,0,0,1};
		Vec4F	mDiffuseLight = { 0,0,0,1 };
		Vec4F	mDiffuseDirection = { 0.33f, 0.33f, 0.33f, 1.0f};

		SMatrix44 mLightView;
		SMatrix44 mLightTransposeView;

		SMatrix44 mLightProjection;
		SMatrix44 mLightTransposeProjection;

		SMatrix44 mLightOrigin;
		SMatrix44 mLightTransposeOrigin;

		SMatrix44 mLightTransposeVPMT;

		SMatrix44 mLightTransposeOriginOrProjection;

		std::mutex mMutex;
		SShaderParam	mAmbientColorHandle;
		SShaderParam	mDiffuseColorHandle;
		SShaderParam	mSpecularColorHandle;
		std::vector<CLightEntity*>	mLights;
		std::vector<CLightEntity*>	mEffectLights;
		std::vector<SLightData>	mLightData;
		bool mbOriginMT = false;
	};
}