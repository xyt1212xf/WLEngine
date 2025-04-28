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
		
		Matrix44& getLightViewMT();
		Matrix44& getLightTransposeViewMT();

		Matrix44& getLightOriginMT();
		Matrix44& getLightProjectionMT();

		Matrix44& getLightTransposeProjectionMT();

		Matrix44& getLightTransposeVPMT();
	protected:
		virtual void _update(UINT32 dTime);
		
	private:
		Vec4F	mAmbientLight = {0,0,0,1};
		Vec4F	mDiffuseLight = { 0,0,0,1 };
		Vec4F	mDiffuseDirection = { 0.33f, 0.33f, 0.33f, 1.0f};

		Matrix44 mLightView;
		Matrix44 mLightTransposeView;

		Matrix44 mLightProjection;
		Matrix44 mLightTransposeProjection;

		Matrix44 mLightOrigin;
		Matrix44 mLightTransposeOrigin;

		Matrix44 mLightTransposeVPMT;

		Matrix44 mLightTransposeOriginOrProjection;

		std::mutex mMutex;
		shaderParam	mAmbientColorHandle;
		shaderParam	mDiffuseColorHandle;
		shaderParam	mSpecularColorHandle;
		std::vector<CLightEntity*>	mLights;
		std::vector<CLightEntity*>	mEffectLights;
		std::vector<LightData>	mLightData;
		bool mbOriginMT = false;
	};
}