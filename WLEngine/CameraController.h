#pragma once
#include "Controller.h"
#include "CameraEntity.h"
#include <functional>

namespace WL
{
	class  CCameraController : public CController
	{
	public:
		CCameraController(CCameraEntity* pCameraEntity);
		void setCameraOnEvent(std::function<void(event&)> function, CameraType type);
		void bindCameraEntity(CCameraEntity* pCameraEntity);
		void strafe(float units); // 左右
		void fly(float units);    // 上下
		void walk(float units);   // 前后
		void pitch(float fRadian);
		void yaw(float angle);   // 旋转up向量
		void roll(float angle);  // 旋转look向量


	protected:
		virtual bool onEvent(event& e) final;
		void _freeOnEvent(event& e);
		void _firstOnEvent(event& e);
		void _thirdOnEvent(event& e);
		void _customOnEvent(event& e);
		std::function<void(event&)> mOnEventFunctions[CameraType::MaxCameraType];

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