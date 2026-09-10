#include "LightEngine.h"
#include "wlEngine.h"
#include "CameraEntity.h"
#include "WLEngine.h"

#define MaxLightNumber 499

namespace WL
{
	CLightEngine::CLightEngine()
	{
		mLightData.resize(MaxLightNumber);
		ZeroMemory(mLightData.data(), sizeof(SLightData) * MaxLightNumber);
		Math::matrixIdentity(&mLightView);
		Math::matrixIdentity(&mLightProjection);
	}

	CLightEngine::~CLightEngine()
	{
	
	}

	void CLightEngine::enableOriginMT()
	{
		mbOriginMT = true;
		mLightTransposeOriginOrProjection = mLightTransposeOrigin;
		mLightTransposeVPMT = mLightTransposeView * mLightTransposeOriginOrProjection;
	}

	void CLightEngine::disableOriginMT()
	{
		mbOriginMT = false;
		mLightTransposeOriginOrProjection = mLightTransposeProjection;
		mLightTransposeVPMT = mLightTransposeView * mLightTransposeOriginOrProjection;
	}

	void CLightEngine::addLightEntity(CLightEntity* pLight)
	{
		if (nullptr != pLight)
		{
			auto iter = std::find(mLights.begin(), mLights.end(), pLight);
			if (mLights.end() == iter)
			{
				mLights.emplace_back(pLight);
				pLight->addRef();
				GEngine->addEntity(pLight);
			}
		}
	}

	void CLightEngine::removeLight(CLightEntity* pLight)
	{
		if (nullptr != pLight)
		{
			auto iter = std::find(mLights.begin(), mLights.end(), pLight);
			if (iter != mLights.end())
			{
				WL_DECREASE((*iter));
				mLights.erase(iter);
				GEngine->removeEntity(pLight);
			}
		}
	}

	bool CLightEngine::initialize()
	{
		auto pLua = GEngine->createLuaState("LightEngine");
		if (pLua)
		{
			GEngine->doFile(pLua, "resource/Material/lightEngine.ma");
			int t = lua_gettop(pLua);
			lua_getglobal(pLua, "material");
			lua_pushstring(pLua, "psConstBuffer");
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				if (lua_istable(pLua, -1))
				{
					INT32 nCount = Foundation::calculateTableLength(pLua);
					int nSize = 0;
					for (INT32 i = 1; i < nCount; ++i)
					{
						t = lua_gettop(pLua);
						lua_pushnumber(pLua, i);
						lua_gettable(pLua, -2);
						if (!lua_isnil(pLua, -1))
						{
							SShaderParam param;
							lua_pushstring(pLua, "name");
							lua_gettable(pLua, -2);
							param.szName = lua_tostring(pLua, -1);

							lua_pop(pLua, 1);

							lua_pushstring(pLua, "size");
							lua_gettable(pLua, -2);
							param.nSize = static_cast<INT32>(lua_tonumber(pLua, -1));
							lua_pop(pLua, 1);

							lua_pushstring(pLua, "registerIndex");
							lua_gettable(pLua, -2);
							param.nRegister = static_cast<INT32>(lua_tonumber(pLua, -1));
							lua_pop(pLua, 1);

							lua_pushstring(pLua, "packoffset");
							lua_gettable(pLua, -2);
							param.packOffset = nSize;
							if (!lua_isnil(pLua, -1))
							{
								param.packOffset = static_cast<INT32>(lua_tonumber(pLua, -1));
							}
							lua_pop(pLua, 2);
							nSize += param.nSize;
							GEngine->addGlobalShaderParam(ShaderBufferType::PS, param);
							if (param.szName == "AmbientLight")
							{
								std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", param.szName.c_str());
								if (strContent.length() > 0)
								{
									auto split = Foundation::splitString<float>(strContent, ",");
									memcpy(&mAmbientLight.x, &split[0], sizeof(mAmbientLight));
								}
							}
							else if (param.szName == "DiffuseLight")
							{
								std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", param.szName.c_str());
								if (strContent.length() > 0)
								{
									auto split = Foundation::splitString<float>(strContent, ",");
									memcpy(&mDiffuseLight.x, &split[0], sizeof(mDiffuseLight));
								}
							}
							else if (param.szName == "LightDirection")
							{
								std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", param.szName.c_str());
								if (strContent.length() > 0)
								{
									auto split = Foundation::splitString<float>(strContent, ",");
									memcpy(&mDiffuseDirection.x, &split[0], sizeof(mDiffuseDirection));
								}
							}
							else if (param.szName == "LightView")
							{
								std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", param.szName.c_str());
								if (strContent.length() > 0)
								{
									auto split = Foundation::splitString<float>(strContent, ",");
									memcpy(&mDiffuseDirection.x, &split[0], sizeof(mDiffuseDirection));
								}
							}
							else if (param.szName == "LightProjection")
							{
								std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", param.szName.c_str());
								if (strContent.length() > 0)
								{
									auto split = Foundation::splitString<float>(strContent, ",");
									memcpy(&mDiffuseDirection.x, &split[0], sizeof(mDiffuseDirection));
								}
							}
						}
						lua_settop(pLua, t);
					}
				}
			}
			GEngine->releaseLuaState("LightEngine");
			GEngine->getLightViewMT();
			GEngine->getLightProjectionMT();
			GEngine->getLightOriginMT();
			GEngine->disableOriginMT();
		}
		return true;
	}

	void CLightEngine::unInitialize()
	{
	}

	void CLightEngine::setAllLights(CMaterialInstance* pMaterialInstance)
	{
		setEnvironmentLight(pMaterialInstance);
		setLightMatrix(pMaterialInstance);
		setDynamicLight(pMaterialInstance);
	}

	INT32 CLightEngine::getLightCount()
	{
		return (INT32)mLights.size();
	}

	INT32 CLightEngine::getLightCountTest()
	{
		return MaxLightNumber - 2;
	}

	void CLightEngine::copyLightData()
	{
		mMutex.lock();
		for (size_t i = 0; i < mLightData.size() && i < mEffectLights.size(); ++i)
		{
			mLightData[i] = mEffectLights[i]->mData;
		}
		mMutex.unlock();
	}

	void CLightEngine::setAmbientLight(const Vec4F& color)
	{
		mAmbientLight = color;
	}

	const Vec4F& CLightEngine::getAmbientLight() const
	{
		return mAmbientLight;
	}

	void CLightEngine::setDiffuseLight(const Vec4F& color)
	{
		mDiffuseLight = color;
	}

	const Vec4F& CLightEngine::getDiffuseLight() const
	{
		return mDiffuseLight;
	}

	void CLightEngine::setDiffuseDirection(const Vec4F& dir)
	{
		mDiffuseDirection = dir;
	}

	const Vec4F& CLightEngine::getDiffuseDirection() const
	{
		return mDiffuseDirection;
	}

	SMatrix44& CLightEngine::getLightViewMT()
	{
		Vec3F lightPos = { mDiffuseDirection.x * 100, mDiffuseDirection.y * 100, mDiffuseDirection.z * 100 };
		static Vec3F lookAt = { 0,0,0 };
		static Vec3F up = { 0, 1, 0};
		
		// Create the view matrix from the three vectors.
		Math::buildMatrixLookAtLH(&mLightView, &lightPos, &lookAt, &up);
		Math::matrixTranspose(&mLightTransposeView, &mLightView);
		return mLightView;
	}

	SMatrix44& CLightEngine::getLightTransposeViewMT()
	{
		return mLightTransposeView;
	}

	SMatrix44& CLightEngine::getLightProjectionMT()
	{
		getLightViewMT();
		// Setup field of view and screen aspect for a square light source.
		static float fieldOfView = (float)PI / 2.0f;
		static float screenAspect = 1.0f;

		Math::buildMatrixPerspectiveFovLH(&mLightProjection, fieldOfView, screenAspect, 1, 1000);
		Math::matrixTranspose(&mLightTransposeProjection, &mLightProjection);
		return mLightProjection;
	}

	SMatrix44& CLightEngine::getLightOriginMT()
	{
		getLightViewMT();
		Math::buildOrthographicMatrixLH(&mLightOrigin, 1024, 1024, 1, 1000);
		Math::matrixTranspose(&mLightTransposeOrigin, &mLightOrigin);
		return mLightOrigin;
	}

	SMatrix44& CLightEngine::getLightTransposeProjectionMT()
	{
		return mLightTransposeOriginOrProjection;
	}

	SMatrix44& CLightEngine::getLightTransposeVPMT()
	{
		return mLightTransposeVPMT;
	}

	void CLightEngine::_update(UINT32 dTime)
	{
		auto* pCamera = GEngine->getMainCameraEntity();
		if (nullptr != pCamera)
		{
			mMutex.lock();
			mEffectLights.clear();
			for (auto item : mLights)
			{
				const auto& sphere = item->getBoundingSphere();
				mEffectLights.emplace_back(item);
			}
			mMutex.unlock();
		}
#if 0
		static float fValue = 0;
		fValue += 0.001f;
		mDiffuseDirection.x = sin(fValue);
		mDiffuseDirection.z = cos(fValue);
		mDiffuseDirection.normalize();
		getLightViewMT();
		getLightProjectionMT();
#endif
	}

	void CLightEngine::setEnvironmentLight(CMaterialInstance* pMaterialInstance)
	{
		pMaterialInstance->setPSParamByName("AmbientLight", &mAmbientLight);
		pMaterialInstance->setPSParamByName("DiffuseLight", &mDiffuseLight);
		pMaterialInstance->setPSParamByName("LightDirection", &mDiffuseDirection);
	}

	void CLightEngine::setLightMatrix(CMaterialInstance* pMaterialInstance)
	{
		pMaterialInstance->setPSParamByName("LightView", &mLightView);
		pMaterialInstance->setPSParamByName("LightProjection", &mLightProjection);
	}

	void CLightEngine::setDynamicLight(CMaterialInstance* pMaterialInstance)
	{
		pMaterialInstance->setStructParamByName("GlobalLight", mLightData.data(), (int)mLightData.size() * sizeof(SLightData));
	}
}