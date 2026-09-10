#pragma once
#include "Vector.h"
#include "Transform.h"
#include "ActorEntity.h"

namespace WL
{
	//灯光模型的
	class CLightModel : public CActorEntity
	{
	public:
		CLightModel();
		virtual ~CLightModel();
	};
	enum LightType
	{
		PointLight,
		DirectionLight,
		SPotLight,
	};
	
	struct SLightData
	{
		Vec3F color;
		float type;
		Vec3F positionOrDirection;
		float fRange;
	};

	class CLightEntity : public CEntity,
						 public CTranformComponet
	{
		friend class CLightEngine;
	public:
		CLightEntity();
		virtual ~CLightEntity();
		void setPosition(const Vec3F& vec);
		const Vec3F& getPosition()const;
		void setDirection(const Vec3F& dir);
		void setColor(const Vec3F& color);
		void setRange(float fRange);
		float getRange();
		void setLightType(LightType type);
		void bindModel(const std::string& szModel);

		//void setRadius(float fRadius);
		//float getRadius();

		void generateViewMT(const Vec3F& lookAt);
		void generateProjectMT(float fNear, float fFar, float fFov = (float)PI/4.0f, float fAspect = 1.0f);
	
		const Vec3F& getDirection() const;
		const Vec3F& getColor() const;
		LightType getLightType();
#ifdef _DEBUG
		void setOriginPosition(const Vec3F& vec);
#endif // DEBUG

	private:
		virtual void _tick(UINT32 dt) final;

	private:
		//显示灯光的外形的模型
		CActorEntity* mpModelInstance = nullptr;
		CMaterialInstance* mpMaterialInstance = nullptr;
		LightType	mLightType = PointLight;
		SMatrix44 mViewMatrix;
		SMatrix44 mProjectMatrix;
		SLightData mData;
#ifdef _DEBUG
		float mfValue = 0;
		float mfSpeed = 0;
		Vec3F mOldPos = { 0, 0, 0 };
#endif
	};
}