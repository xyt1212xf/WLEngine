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
		void strafe(float units); // ����
		void fly(float units);    // ����
		void walk(float units);   // ǰ��
		void pitch(float fRadian);
		void yaw(float angle);   // ��תup����
		void roll(float angle);  // ��תlook����


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