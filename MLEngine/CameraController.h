#pragma once
#include "Controller.h"
#include "CameraEntity.h"
#include <functional>

namespace WL
{
	class  CCameraController : public SController
	{
	public:
		CCameraController(CCameraEntity* pCameraEntity);
		void setCameraOnEvent(std::function<void(SEvent&)> function, CameraType type);
		void bindCameraEntity(CCameraEntity* pCameraEntity);
		void strafe(float units); // 左右
		void fly(float units);    // 上下
		void walk(float units);   // 前后
		void pitch(float fRadian);
		void yaw(float angle);   // 旋转up向量
		void roll(float angle);  // 旋转look向量


	protected:
		virtual bool onEvent(SEvent& e) final;
		void _freeOnEvent(SEvent& e);
		void _firstOnEvent(SEvent& e);
		void _thirdOnEvent(SEvent& e);
		void _customOnEvent(SEvent& e);
		std::function<void(SEvent&)> mOnEventFunctions[CameraType::MaxCameraType];

	protected:
		CCameraEntity* mpCameraEntity = nullptr;
		int mMouseFlag = 0;
		int mMouseX = 0;
		int mMouseY = 0;
		float mMouseWheelSpeed = 100.0f;
		float mMoveSpeed = 1000.0f;
		float mRiseSpeed = 1.0f;
		float mLockXAxisAngle = 60;
	};
}