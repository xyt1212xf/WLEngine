#include "CameraController.h"
#include "WLEngine.h"
#include "Scene.h"
#include "Message.h"

namespace WL
{
	void processMouseWheel(CCameraEntity* pCameraEntity, bool bDown, float speed)
	{
		auto dir = pCameraEntity->getLookAt() - pCameraEntity->getEye();
		dir.normalize();
		if (bDown)
		{
			dir *= speed;
		}
		else
		{
			dir *= -speed;
		}

		auto eye = pCameraEntity->getEye() + dir;
		dir = eye - pCameraEntity->getLookAt();
		if (dir.lengthSqrt() > 4)
		{
			pCameraEntity->setEye(eye);
			pCameraEntity->buildViewMatrix();
		}
	}

	CCameraController::CCameraController(CCameraEntity* pCameraEntity)
	{
		bindCameraEntity(pCameraEntity);
	}

	void CCameraController::setCameraOnEvent(std::function<void(event&)> function, CameraType type)
	{
		mOnEventFunctions[type] = function;
	}

	void CCameraController::bindCameraEntity(CCameraEntity* pCameraEntity)
	{
		if (pCameraEntity)
		{
			mpCameraEntity = pCameraEntity;
			mpCameraEntity->setCameraType(Third);

			mOnEventFunctions[CameraType::Free] = std::bind(&CCameraController::_freeOnEvent, this, std::placeholders::_1);
			mOnEventFunctions[CameraType::First] = std::bind(&CCameraController::_firstOnEvent, this, std::placeholders::_1);
			mOnEventFunctions[CameraType::Third] = std::bind(&CCameraController::_thirdOnEvent, this, std::placeholders::_1);
			mOnEventFunctions[CameraType::Custom] = std::bind(&CCameraController::_customOnEvent, this, std::placeholders::_1);
		}
	}

	void CCameraController::strafe(float units)
	{
		mpCameraEntity->mEye.x -= mpCameraEntity->mLeftDirection.x * units;
		mpCameraEntity->mEye.z -= mpCameraEntity->mLeftDirection.z * units;
		mpCameraEntity->mLookAt.x -= mpCameraEntity->mLeftDirection.x * units;
		mpCameraEntity->mLookAt.z -= mpCameraEntity->mLeftDirection.z * units;
	}

	void CCameraController::fly(float units)
	{
		mpCameraEntity->mEye.y += units;
		mpCameraEntity->mLookAt.y += units;
	}

	void CCameraController::walk(float units)
	{
		mpCameraEntity->mEye += (mpCameraEntity->mFrontDirection * units);
		mpCameraEntity->mLookAt += mpCameraEntity->mFrontDirection * units;
	}

	void CCameraController::pitch(float fRadian)
	{

	}

	void CCameraController::yaw(float angle)
	{

	}

	void CCameraController::roll(float angle)
	{

	}

	bool CCameraController::onEvent(event& e)
	{
		if (nullptr != mpCameraEntity)
		{
			switch (e.message)
			{
			case MSG_MOUSEMOVE:
			{
				CRay ray(mpCameraEntity);
				ray.setScreenMouseSpace(e.mouseX, e.mouseY);
				auto pActors = GEngine->getCurrentScenePtr()->getShowActors(Actor);
				CActorEntity* pPick = nullptr;
				float fDistance = FLT_MAX;
				for( auto item : pActors )
				{
					if (ray.intersectActorEntity(item))
					{
						if (fDistance < ray.mResult.fDist)
						{
							pPick = item;
							fDistance = ray.mResult.fDist;
						}
					}
				}
				break;
			}
			}
			auto type = mpCameraEntity->getCameraType();
			switch (type)
			{
			case CameraType::Free:
			case CameraType::First:
			case CameraType::Third:
			case CameraType::Custom:
			{
				mOnEventFunctions[type](e);
			}
			default:
				return false;
			}
		}
		return true;
	}

	void CCameraController::_freeOnEvent(event& e)
	{
		switch (e.message)
		{
		case MSG_KEYDOWN:
		{
			switch (e.wParam)
			{
			case 'Q':
			case 'q':
			{
				fly(-mRiseSpeed);
				break;
			}
			case 'E':
			case 'e':
			{
				fly(mRiseSpeed);
				break;
			}
			case 'W':
			case 'w':
			{
				walk(mMoveSpeed);
				break;
			}
			case 'S':
			case 's':
			{
				walk(-mMoveSpeed);
				break;
			}
			case 'A':
			case 'a':
			{
				strafe(-mMoveSpeed);
				break;
			}
			case 'D':
			case 'd':
			{
				strafe(mMoveSpeed);
				break;
			}
			}
			mpCameraEntity->buildViewMatrix();
			break;
		}
		case MSG_KEYUP:
		{
			break;
		}
		case MSG_LBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 0;
			break;
		}
		case MSG_LBUTTONUP:
		{
			mMouseFlag ^= 1 << 0;
			break;
		}
		case MSG_RBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 1;
			break;
		}
		case MSG_RBUTTONUP:
		{
			mMouseFlag ^= 1 << 1;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if (mMouseFlag & (1 << 1))
			{
				int x = mMouseX - e.mouseX;
				int y = e.mouseY - mMouseY;

				mMouseX = e.mouseX;
				mMouseY = e.mouseY;
				auto dir = mpCameraEntity->mLookAt - mpCameraEntity->mEye;
				Matrix44 xMatrix, yMatrix;
				Math::matrixRotationY(&yMatrix, x * mpCameraEntity->mMoveSpeed);
				dir = Math::matrixMulVec3(xMatrix, dir);

				static float yValue = 0;
				float fTemp = yValue + y * mpCameraEntity->mMoveSpeed;
				if (fTemp > 0.1f)
				{
					yValue = 0.1f;
				}
				else if (fTemp < -0.1f)
				{
					yValue = -0.1f;
				}
				else
				{
					yValue = fTemp;
					Math::matrixRotationX(&xMatrix, y * mpCameraEntity->mMoveSpeed);
				}


				//dir = Math::matrixMulVec3(xMatrix, dir);
			//	{
			//		auto dirEx = mpCameraEntity->mLookAt - mpCameraEntity->mEye;
			//		Math::vec3RotationY(&dirEx, x * mpCameraEntity->mMoveSpeed);
			////		Math::vec3RotationX(&dirEx, -y * mpCameraEntity->mMoveSpeed);
			//		int a = 0;
			//	}
				mpCameraEntity->mLookAt = mpCameraEntity->mEye + dir;
				mpCameraEntity->buildViewMatrix();
			}
			break;
		}
		case MSG_MOUSEWHEEL:
		{
#ifdef _WIN32
			processMouseWheel(mpCameraEntity, 120 == (short)HIWORD(e.wParam), mMouseWheelSpeed);
#endif
			break;
		}
		default:
			break;
		}
	}

	void CCameraController::_firstOnEvent(event& e)
	{
		switch (e.message)
		{
		case MSG_KEYDOWN:
		{
			switch (e.wParam)
			{
			case 'Q':
			case 'q':
			{
				mpCameraEntity->mEye.y -= mRiseSpeed;
				mpCameraEntity->mLookAt.y -= mRiseSpeed;
				break;
			}
			case 'E':
			case 'e':
			{
				mpCameraEntity->mEye.y += mRiseSpeed;
				mpCameraEntity->mLookAt.y += mRiseSpeed;
				break;
			}
			case 'W':
			case 'w':
			{
				mpCameraEntity->mEye.x += mpCameraEntity->mFrontDirection.x * mMoveSpeed;
				mpCameraEntity->mEye.z += mpCameraEntity->mFrontDirection.z * mMoveSpeed;
				mpCameraEntity->mLookAt.x += mpCameraEntity->mFrontDirection.x * mMoveSpeed;
				mpCameraEntity->mLookAt.z += mpCameraEntity->mFrontDirection.z * mMoveSpeed;
				break;
			}
			case 'S':
			case 's':
			{
				mpCameraEntity->mEye.x -= mpCameraEntity->mFrontDirection.x * mMoveSpeed;
				mpCameraEntity->mEye.z -= mpCameraEntity->mFrontDirection.z * mMoveSpeed;
				mpCameraEntity->mLookAt.x -= mpCameraEntity->mFrontDirection.x * mMoveSpeed;
				mpCameraEntity->mLookAt.z -= mpCameraEntity->mFrontDirection.z * mMoveSpeed;
				break;
			}
			case 'A':
			case 'a':
			{
				mpCameraEntity->mEye.x -= mpCameraEntity->mLeftDirection.x * mMoveSpeed;
				mpCameraEntity->mEye.z -= mpCameraEntity->mLeftDirection.z * mMoveSpeed;
				mpCameraEntity->mLookAt.x -= mpCameraEntity->mLeftDirection.x * mMoveSpeed;
				mpCameraEntity->mLookAt.z -= mpCameraEntity->mLeftDirection.z * mMoveSpeed;
				break;
			}
			case 'D':
			case 'd':
			{
				mpCameraEntity->mEye.x += mpCameraEntity->mLeftDirection.x * mMoveSpeed;
				mpCameraEntity->mEye.z += mpCameraEntity->mLeftDirection.z * mMoveSpeed;
				mpCameraEntity->mLookAt.x += mpCameraEntity->mLeftDirection.x * mMoveSpeed;
				mpCameraEntity->mLookAt.z += mpCameraEntity->mLeftDirection.z * mMoveSpeed;
				break;
			}
			}
			mpCameraEntity->buildViewMatrix();
			break;
		}
		case MSG_KEYUP:
		{
			break;
		}
		case MSG_LBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 0;
			break;
		}
		case MSG_LBUTTONUP:
		{
			mMouseFlag ^= 1 << 0;
			break;
		}
		case MSG_RBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 1;
			break;
		}
		case MSG_RBUTTONUP:
		{
			mMouseFlag ^= 1 << 1;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if (mMouseFlag & (1 << 1))
			{
				int x = e.mouseX - mMouseX;
				int y = e.mouseY - mMouseY;

				mMouseX = e.mouseX;
				mMouseY = e.mouseY;

				auto dir = mpCameraEntity->mLookAt - mpCameraEntity->mEye;

				Math::vec3RotationY(&dir, x * mpCameraEntity->mMoveSpeed);
				Math::vec3RotationX(&dir, y * mpCameraEntity->mMoveSpeed);
				mpCameraEntity->mLookAt = mpCameraEntity->mEye + dir;
				mpCameraEntity->buildViewMatrix();
			}
			break;
		}
		case  MSG_MOUSEWHEEL:
		{
#ifdef _WIN32
			processMouseWheel(mpCameraEntity, 120 == (short)HIWORD(e.wParam), mMouseWheelSpeed);
#endif
			break;
		}
		default:
			break;
		}
	}

	void CCameraController::_thirdOnEvent(event& e)
	{
		switch (e.message)
		{
		case MSG_KEYDOWN:
		{
			switch (e.wParam)
			{
			case 'Q':
			case 'q':
			{
				fly(-mRiseSpeed);
				break;
			}
			case 'E':
			case 'e':
			{
				fly(mRiseSpeed);
				break;
			}
			case 'W':
			case 'w':
			{
				walk(mMoveSpeed);
				break;
			}
			case 'S':
			case 's':
			{
				walk(-mMoveSpeed);
				break;
			}
			case 'A':
			case 'a':
			{
				strafe(mMoveSpeed);
				break;
			}
			case 'D':
			case 'd':
			{
				strafe(-mMoveSpeed);
				break;
			}
			}
			mpCameraEntity->buildViewMatrix();
			break;
		}
		case MSG_KEYUP:
		{
			break;
		}
		case MSG_LBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 0;
			break;
		}
		case MSG_LBUTTONUP:
		{
			mMouseFlag ^= 1 << 0;
			break;
		}
		case MSG_RBUTTONDOWN:
		{
			mMouseX = e.mouseX;
			mMouseY = e.mouseY;
			mMouseFlag |= 1 << 1;
			break;
		}
		case MSG_RBUTTONUP:
		{
			mMouseFlag ^= 1 << 1;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if (mMouseFlag & (1 << 1))
			{
				int x = e.mouseX - mMouseX;
				int y = e.mouseY - mMouseY;

				auto dir = mpCameraEntity->mEye - mpCameraEntity->mLookAt;
				Math::vec3RotationY(&dir, x * mpCameraEntity->mMoveSpeed);
				mpCameraEntity->mEye = mpCameraEntity->mLookAt + dir;
				mpCameraEntity->buildViewMatrix();
				mMouseX = e.mouseX;
				mMouseY = e.mouseY;
			}
			break;
		}
		case MSG_MOUSEWHEEL:
		{
#ifdef _WIN32
			processMouseWheel(mpCameraEntity, 120 == (short)HIWORD(e.wParam), mMouseWheelSpeed);
#endif
			break;
		}
		default:
			break;
		}
	}

	void CCameraController::_customOnEvent(event& e)
	{

	}

}

