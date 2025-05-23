#include <fstream>
#include "Scene.h"
#include "Foundation.h"
#include "LuaBind.h"
#include "BehaviorTree.h"
namespace WL
{
	static float fViewMaxLength = 100000000;
	int loadScene(const char* szFile, bool bLoadLight)
	{
		auto pScene = GEngine->createScene(szFile);
		if (bLoadLight)
		{
			for (int i = 0; i < GEngine->getLightCountTest(); ++i)
			{
				CLightEntity* pLight = GEngine->createEntity<CLightEntity>(EntityType::Light);
				if (nullptr != pLight)
				{
					pLight->bindModel(R"(resource\Model\sphere.md)");
					pLight->setRange(100.0f + rand() % 50);
					pLight->setLightType(PointLight);
					pLight->setEntityName("");
					pLight->setPosition(Vec3F(rand() % 500 + 0.0f, 1000.0f, 1625.0f + rand() % 150));
					pLight->setOriginPosition(pLight->getPosition());
					pLight->setColor(Vec3F(rand() % 100 / 255.0f, rand() % 100 / 255.0f, rand() % 100 / 255.0f));
					GEngine->getCurrentScenePtr()->addLight(pLight);
					WL_DECREASE(pLight);
				}
			}
		}
		if (pScene)
		{
			auto pConfig = GEngine->getWindowConfigPtr();
			if (pConfig->bShowFps)
			{
				bool bValue = true;
				GEngine->executeCmd("Fps", &bValue);
			}
		}
		return 0;
	}

	bool gotoScene(const char* szFile, bool bLoadLight)
	{
		loadScene(szFile, bLoadLight);
		return true;
	}

	DefineScriptClass(CScene)
	CScene::CScene()
	{

	}

	CScene::~CScene()
	{
		for (auto item : mEntities)
		{
			WL_DECREASE(item);
		}
		mEntities.clear();
	}

	void CScene::registerClass()
	{

	}

	void CScene::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<int>::registrFun<int(*)(const char*, bool), &loadScene, const char*, bool>(pLua, "loadScene");
		CRegisterFun<bool>::registrFun<bool(*)(const char*, bool), &gotoScene, const char*, bool>(pLua, "gotoScene");
		//CBehaviorTreeMgr::regBTreeToScrpit();
	}

	
	std::list<CGuiEntity*>& CScene::getGuis()
	{
		return mGuis;
	}

	std::list<CLightEntity*>& CScene::getLights()
	{
		return mLights;
	}

	std::list<CCameraEntity*>& CScene::getCameras()
	{
		return mCameras;
	}

	std::list<CActorEntity*>& CScene::getActors()
	{
		return mActors;
	}


	std::list<CActorEntity*>& CScene::getShowActors(UINT32 nFlag /*= 0xFFFFFFFF*/)
	{
		if (0xFFFFFFFF == nFlag)
		{
			return mShowActors;
		}
		else
		{
			mPickActors.clear();
			for( auto item : mShowActors )
			{
				if (item->getEntityType() & nFlag)
				{
					mPickActors.push_back(item);
				}
			}
			return mPickActors;
		}
	}

	CCameraEntity* CScene::getMainCamera() const
	{
		return mpMainCamera;
	}

	bool CScene::initialise(const char* szfile)
	{
		mSceneName = szfile;
		mSceneName.append("/");
		std::string content(Foundation::readJsonFile(mSceneName + ("Map.scene")));
		auto js = json::parse(content.c_str());
		auto config = GEngine->getWindowConfigPtr();
		for (auto& item : js.items())
		{
			const auto& k = item.key();
			auto& v = item.value();
			if ("Camera" == k)
			{
				initCamera(v, config);
			}
			else if ("Actor" == k)
			{
				initEntities(v, config);
			}
			else if ("Light" ==k)
			{
				initLight(v, config);
			}
			else if ("Sky" == k)
			{
				initSky(v, config);
			}
			else if ("SkyBox" == k)
			{
				initSkyBox(v, config);
			}
			else if ("BornIn" == k)
			{
				initBornIn(v, config);
			}
			else if ("Terrain" == k)
			{
				initTerrain(v, config);
			}
			else if ("Gui" == k)
			{
				initGui(v, config);
			}
		}	
		return true;
	}


	bool CScene::addEntity(CEntity* pEntity)
	{
		if (nullptr != pEntity)
		{
			GEngine->addEntity(pEntity);
			auto iter = std::find(mEntities.begin(), mEntities.end(), pEntity);
			if (iter == mEntities.end())
			{
				mEntities.emplace_back(pEntity);
				pEntity->addRef();
				switch (pEntity->getEntityType())
				{
				case Light:
					mLights.emplace_back(dynamic_cast<CLightEntity*>(pEntity));
					return true;
				case Camera:
					mCameras.emplace_back(dynamic_cast<CCameraEntity*>(pEntity));
					return true;
				case Ocean:
					mpOceanEntity = dynamic_cast<COceanEntity*>(pEntity);
					break;
				case Sky:
					mpSkyEntity = dynamic_cast<CSkyEntity*>(pEntity);
					break;
				case SkyBox:
					mpSkyBoxEntity = dynamic_cast<CSkyBoxEntity*>(pEntity);
					break;
				case Terrain:
					mpTerrainEntity = dynamic_cast<CTerrainEntity*>(pEntity);
					break;
				case Gui:
					mGuis.emplace_back(dynamic_cast<CGuiEntity*>(pEntity));
					break;
				}
				auto pActorEntity = dynamic_cast<CActorEntity*>(pEntity);
				if (nullptr != pActorEntity)
				{
					mActors.emplace_back(pActorEntity);
					if (pEntity->isVisible())
					{
						mShowActors.emplace_back(mActors.back());
					}
				}
				return true;
			}
		}
		return false;
	}

	bool CScene::removeEntity(CEntity* pEntity)
	{
		if (nullptr != pEntity)
		{
			GEngine->removeEntity(pEntity);
			auto iter = std::find(mEntities.begin(), mEntities.end(), pEntity);
			if (iter != mEntities.end())
			{
				do
				{
					{
						auto _child = std::find(mActors.begin(), mActors.end(), pEntity);
						if (_child != mActors.end())
						{
							mActors.remove(*_child);
						}
					}
					{
						auto _child = std::find(mGuis.begin(), mGuis.end(), pEntity);
						if (_child != mGuis.end())
						{
							mGuis.remove(*_child);
							continue;
						}
					}

					{
						auto _child = std::find(mLights.begin(), mLights.end(), pEntity);
						if (_child != mLights.end())
						{
							mLights.remove(*_child);
							continue;
						}
					}

					{
						auto _child = std::find(mShowActors.begin(), mShowActors.end(), pEntity);
						if (_child != mShowActors.end())
						{
							mShowActors.remove(*_child);
							continue;
						}
					}
					{
						auto _child = std::find(mCameras.begin(), mCameras.end(), pEntity);
						if (_child != mCameras.end())
						{
							mCameras.remove(*_child);
						}
					}
				} while (false);
				mEntities.erase(iter);
				WL_DECREASE(pEntity);
				return true;
			}
		}
		return false;
	}


	void CScene::addLight(CLightEntity* pLight)
	{
		if (nullptr != pLight)
		{
			GEngine->addLightEntity(pLight);
			mEntities.emplace_back(pLight);
		}
	}


	void CScene::removeLight(CLightEntity* pLight)
	{
		if (nullptr != pLight)
		{
			GEngine->removeLight(pLight);
		}
	}

	void CScene::setVoxelMgr(CVoxelMgr* pVoxelMgr)
	{
		if (mpVoxelMgr != pVoxelMgr)
		{
			WL_DECREASE(mpVoxelMgr);
		}
		mpVoxelMgr = pVoxelMgr;
		mpVoxelMgr->addRef();
	}



	void CScene::draw(UINT32 dTime)
	{
		for (auto& item : mCameras)
		{
			item->clearDrawEntities();
			item->addDrawEntities(mShowActors, dTime);
		}
		for (auto& item : mCameras)
		{
			auto& drawEntitiesArray = item->getDrawEntities();
			GEngine->drawEntity(drawEntitiesArray, dTime);
		}
	}

	void CScene::_update(UINT32 dt)
	{
		if(nullptr != mpMainCamera)
		{
			checkVoxleChunk();
		}
		auto pVoxelMgr = GEngine->getVoxelMgr();
		for (auto& item : mCameras)
		{
			const auto& eye = item->getEye();
			for (const auto& [k, v] : mChunks)
			{
				for( auto& voxel : v.voxels )
				{
					if (!v.isLoad)
					{
						if (Math::getLengthsqr(eye, voxel) < fViewMaxLength)
						{
							pVoxelMgr->createChunk(k, v.voxels);
						}
					}
				}
			}
		}
		for (auto item : mEntities)
		{
			item->_tick(dt);
		}
	}

	//bool CScene::processInputEvent(event& msg)
	//{
	//	auto pScene = GEngine->getCurrentScenePtr();
	//	if (nullptr != pScene)
	//	{
	//		pScene->mpGameCtrl->process(msg);
	//	}
	//	return false;
	//}

	//void CScene::bindGameControl(CGameControl* pGameCtrl)
	//{
	//	SafeDelete(mpGameCtrl);
	//	mpGameCtrl = pGameCtrl;
	//}

	void CScene::initCamera(const json::reference& value, WindowConfig* config)
	{
		Vec3F eye, lookAt, up;
		float fov, width, height, nearPlane, farPlane;
		CCameraEntity* pCamera = GEngine->createEntity<CCameraEntity>(EntityType::Camera);
		
		for (const auto& [k, v] : value.items())
		{
			if ("name" == k)
			{
				pCamera->setEntityName(v.get<std::string>());
			}
			else if ("eye" == k)
			{
				std::string szContent(v.get<std::string>());
				auto valueArray = Foundation::splitString<float>(szContent, ",");
				eye = Vec3F(&valueArray[0]);
				pCamera->setEye(Vec3F(eye[0], eye[1], eye[2]));
			}
			else if ("lookAt" == k)
			{
				std::string szContent(v.get<std::string>());
				auto valueArray = Foundation::splitString<float>(szContent, ",");
				lookAt = Vec3F(&valueArray[0]);
				pCamera->setLookAt(Vec3F(lookAt[0], lookAt[1], lookAt[2]));
			}
			else if ("up" == k)
			{
				std::string szContent(v.get<std::string>());
				auto valueArray = Foundation::splitString<float>(szContent, ",");
				up = Vec3F(&valueArray[0]);
			}
			else if ("fov" == k)
			{
				fov = v.get<float>();
			}
			else if ("aspect" == k)
			{
				std::string szContent(v.get<std::string>());
				auto valueArray = Foundation::splitString<float>(szContent, ",");
				width = valueArray[0];
				height = valueArray[1];
			}
			else if ("cullPlane" == k)
			{
				std::string szContent(v.get<std::string>());
				auto valueArray = Foundation::splitString<float>(szContent, ",");
				nearPlane = valueArray[0];
				farPlane = valueArray[1];
			}
		}
		pCamera->buildViewMatrix();
		pCamera->buildPerspectiveFovMatrix(fov, (float)config->nWidth, (float)config->nHeight, nearPlane, farPlane);

		addEntity(pCamera);
		mpMainCamera = pCamera;
		WL_DECREASE(pCamera);
	}

	UINT32 createEntity(EntityType type)
	{
		auto pEntity = GEngine->createEntity(type);
		if( nullptr != pEntity )
		{
			return pEntity->getID();
		}
		return 0;

	}
	void CScene::initEntities(const json& value, WindowConfig* config)
	{
		bool bVisible = false;
		for (const auto& actor : value)
		{
			CActorEntity* pEntity = nullptr;
			if (actor.contains("type"))
			{
				EntityType type = static_cast<EntityType>(actor["type"]);
				switch (type)
				{
				case Actor:
					pEntity = GEngine->createEntity<CActorEntity>(type);
					initActor(actor, pEntity);
					break;
				case Ocean:
					pEntity = GEngine->createEntity<COceanEntity>(type);
					initOcean(actor, pEntity);
					break;
				default:
					break;
				}
			}
		}
		//TiXmlElement* pChildElement = pType->FirstChildElement();
		//while (pChildElement)
		//{
		//	std::map<std::string, std::string> MeshInfo;
		//	TiXmlAttribute* Attribute = pChildElement->FirstAttribute();  
		//	while (Attribute != nullptr)
		//	{
		//		MeshInfo[Attribute->Name()] = Attribute->Value();          
		//		Attribute = Attribute->Next();
		//	}

		//	//CRenderNode* pRenderNode = GEngine->createRenderNode();
		//	//std::transform(MeshInfo["Mesh"].begin(), MeshInfo["Mesh"].end(), MeshInfo["Mesh"].begin(), ::tolower);
		//	CActorEntity* pActor = GEngine->createEntity<CActorEntity>(EntityType::Actor);

		//	GEngine->loadActorEntity(pActor, MeshInfo["Model"]);
		//	if (1 == std::stoi(MeshInfo["Visible"]))
		//	{
		//		pActor->show();
		//	}
		//	else
		//	{
		//		pActor->hide();
		//	}

		//	auto&& strScale = Foundation::split(MeshInfo["Scale"], ",");
		//	std::vector<float> scale;
		//	scale.reserve(strScale.size());
		//	for (auto& item : strScale)
		//	{
		//		scale.push_back(std::stof(item));
		//	}
		//	pActor->setScale(Vec3F(&scale.front()));

		//	auto&& strRotation = Foundation::split(MeshInfo["Rotation"], ",");
		//	std::vector<float> rotation;
		//	rotation.reserve(strRotation.size());
		//	for (auto& item : strRotation)
		//	{
		//		rotation.push_back(std::stof(item));
		//	}
		//	pActor->setRotation(Vec3F(&rotation.front()));

		//	auto&& strVec = Foundation::split(MeshInfo["Position"], ",");
		//	std::vector<float> vec;
		//	vec.reserve(strVec.size());
		//	for (auto& item : strVec)
		//	{
		//		vec.push_back(std::stof(item));
		//	}
		//	pActor->setPosition(Vec3F(&vec.front()));


		//	pActor->setName(MeshInfo["Name"]);

		//	//SafeRelease(pRenderNode);
		//	pChildElement = pChildElement->NextSiblingElement();
		//	addEntity(pActor);
		//	WL_DECREASE(pActor);
		//	//	,
		//	//	(EntityType)std::stoi(MeshInfo["Type"]));
		//	//auto iter = MeshInfo.find("Animation");
		//	//if (iter != MeshInfo.end())
		//	//{
		//	//	pActor->loadAnimation(iter->second);
		//	//}
		//	//pRenderNode->bindActor(pActor, RendererType::SkinRenderer | RendererType::FrontRenderer | RendererType::DeferredRenderer | RendererType::ShadowRenderer);
		//	//addRenderNode(pRenderNode);
		//	//pRenderNode->setRenderLayer(99);
		//	//pActor->setScale(CVector(1, 1, 1));
		//}
	}

	void CScene::initChunks(const json& value, WindowConfig* config)
	{
		auto pVoxelMgr = GEngine->getVoxelMgr();
		if (nullptr != pVoxelMgr)
		{
			try
			{
				std::string szContent;
				Vec3F position;
				std::list<Vec3F> voxels;
				for (const auto& tiles : value)
				{
					if (tiles.contains("position"))
					{
						szContent = (tiles["position"]);
						auto value = Foundation::splitString<float>(szContent, ",");
						position = value.data();
					}
					if (tiles.contains("id"))
					{
#ifdef _DEBUG
						szContent = mSceneName;
						szContent.append(tiles["id"]).append(".json");
						auto buffer = std::move(Foundation::readJsonFile(szContent));
						auto js = json::parse(buffer);
						voxels.clear();
						for (auto& item : js.items())
						{
							for (const auto& node : item.value())
							{
								std::string texture = node["texture"];
								std::string sz = node["position"];
								auto value = Foundation::splitString<float>(sz, ",");
								voxels.push_back(Vec3F(value.data()));
							}
						}
#else
						szContent = mSceneName.append(tiles["id"]);
						szContent.append(".dt");
#endif
					}
					position *= (CVoxelChunk::nSize * Voxel::nSize);
					ChunkInfo info = { false, 1, std::move(voxels) };
					float fLength = Math::getLengthsqr(position, mBornPos);
					if (fLength < fViewMaxLength)
					{
						info.isLoad = true;
						pVoxelMgr->createChunk(position, info.voxels);
					}
					else
					{
						mChunks[position] = std::move(info);
					}
				}
			}
			catch (const json::type_error& msg) 
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 4);
				auto pError = msg.what();
				std::cout << pError;
				SetConsoleTextAttribute(hConsole, 7);
			}
		}
	}
	
	void CScene::initLight(const json::reference& value, WindowConfig* config)
	{
		CSkyEntity* pSky = GEngine->createEntity<CSkyEntity>(EntityType::Sky);
		std::string szModel;
		bool bVisible = false;
		for (const auto& [k, v] : value.items())
		{
			if ("children" == k)
			{
				for( const auto& obj: v)
				{
					CLightEntity* pLight = GEngine->createEntity<CLightEntity>(EntityType::Light);
					if (obj.contains("name"))
					{
						pLight->setEntityName(obj["name"]);
					}
					if (obj.contains("lightType"))
					{
						if (PointLight == obj["lightType"])
						{
							std::string szContent(obj["direction"]);
							auto value = Foundation::splitString<float>(szContent, ",");
							pLight->setPosition(Vec3F(&value.front()));
#ifdef _DEBUG	
							pLight->setOriginPosition(Vec3F());
#endif // _DEBUG
						}
						else if (DirectionLight == obj["lightType"])
						{
							if (obj.contains("direction"))
							{
								std::string szContent(obj["direction"]);
								auto value = Foundation::splitString<float>(szContent, ",");
								pLight->setDirection(Vec3F(&value[0]));
							}
						}
					}

					if (obj.contains("range"))
					{
						pLight->setRange(obj["range"]);
					}

					if (obj.contains("color"))
					{
						std::string szContent(obj["color"]);
						auto value = Foundation::splitString<float>(szContent, ",");
						pLight->setColor(Vec3F(&value.front()));
					}
					addLight(pLight);
					WL_DECREASE(pLight);
				}
			}
		}
//		std::map<std::string, std::string> lightInfo;
//		TiXmlElement* pChildElement = pType->FirstChildElement();
//		while (pChildElement != nullptr)
//		{
//			TiXmlAttribute* Attribute = pChildElement->FirstAttribute();  //��ȡbrotherԪ�صĵ�һ������
//			while (Attribute != nullptr)
//			{
//				lightInfo[Attribute->Name()] = Attribute->Value();            //��ȡAttribute���Ե�ֵ
//				Attribute = Attribute->Next();
//			}
//
//			CLightEntity* pLight = GEngine->createEntity<CLightEntity>(EntityType::Light);
//			if (nullptr != pLight)
//			{
//				pLight->bindModel(R"(resource\Model\sphere.md)");
//				auto type = (LightType)std::stoi(lightInfo["LightType"]);
//				pLight->setLightType(type);
//				pLight->setName(lightInfo["Name"]);
//				//CRenderNode* pRenderNode = GEngine->createRenderNode();
//				//pRenderNode->bindActor(pLight, RendererType::SkinRenderer | RendererType::FrontRenderer | RendererType::DeferredRenderer | RendererType::ShadowRenderer);
//				//addRenderNode(pRenderNode);
//				//auto iter = lightInfo.find("Shadwow");
//				//if (iter != lightInfo.end())
//				//{
//				//	if (1 == std::stoi(lightInfo["Shadow"]))
//				//	{
//				//		pLight->createDepthTexture(1024, 1024);
//				//	}
//				//}
//				auto iter = lightInfo.find("Range");
//				if (iter != lightInfo.end())
//				{
//					pLight->setRange(std::stof(iter->second));
//				}
//				if (PointLight == type)
//				{
//					auto&& strVec = Foundation::split(lightInfo["Position"], ",");
//					if (strVec.size() > 0)
//					{
//						std::vector<float> vec;
//						vec.reserve(strVec.size());
//						for (auto& item : strVec)
//						{
//							vec.push_back(std::stof(item));
//						}
//						pLight->setPosition(Vec3F(&vec.front()));
//#ifdef _DEBUG	
//						pLight->setOriginPosition(Vec3F());
//#endif // _DEBUG
//					}
//				}
//				else if (DirectionLight == type)
//				{
//					auto&& strVec = Foundation::split(lightInfo["Direction"], ",");
//					if (strVec.size() > 0)
//					{
//						std::vector<float> vec;
//						vec.reserve(strVec.size());
//						for (auto& item : strVec)
//						{
//							vec.push_back(std::stof(item));
//						}
//						pLight->setDirection(Vec3F(&vec.front()));
//					}
//				}
//				{
//					auto&& strVec = Foundation::split(lightInfo["Color"], ",");
//					if (strVec.size() > 0)
//					{
//						std::vector<float> vec;
//						vec.reserve(strVec.size());
//						for (auto& item : strVec)
//						{
//							vec.push_back(std::stof(item));
//						}
//						pLight->setColor(Vec3F(&vec.front()));
//					}
//				}
//				/*			{
//								auto&& strVec = Foundation::split(lightInfo["AmbientColor"], ",");
//								if (strVec.size() > 0)
//								{
//									std::vector<float> vec;
//									vec.reserve(strVec.size());
//									for (auto& item : strVec)
//									{
//										vec.push_back(std::stof(item));
//									}
//									pLight->setAmbientColor(&vec[0]);
//								}
//							}
//							{
//								auto&& strVec = Foundation::split(lightInfo["DiffuseColor"], ",");
//								if (strVec.size() > 0)
//								{
//									std::vector<float> vec;
//									vec.reserve(strVec.size());
//									for (auto& item : strVec)
//									{
//										vec.push_back(std::stof(item));
//									}
//									pLight->setDiffuseColor(&vec[0]);
//								}
//							}
//
//							{
//								auto&& strVec = foundation::split(lightInfo["SpecularColor"], ",");
//								std::vector<float> vec;
//								if (strVec.size() > 0)
//								{
//									vec.reserve(strVec.size());
//									for (auto& item : strVec)
//									{
//										vec.push_back(std::stof(item));
//									}
//									pLight->setSpecularColor(&vec[0]);
//								}
//							}*/
//				addLight(pLight);
//			}
//			WL_DECREASE(pLight);
//			pChildElement = pChildElement->NextSiblingElement();
//		}
	}

	void CScene::initSky(const json::reference& value, WindowConfig* config)
	{
		CSkyEntity* pSky = GEngine->createEntity<CSkyEntity>(EntityType::Sky);

		std::string szModel;
		bool bVisible = false;
		Vec3F position, scale, rotation;
		for (const auto& [k, v] : value.items())
		{
			if ("name" == k)
			{
				pSky->setEntityName(v.get<std::string>());
			}
			else if ("position" == k)
			{
				std::string szContent(v.get<std::string>());
				auto value = Foundation::splitString<float>(szContent, ",");
				position = Vec3F(&value[0]);
			}
			else if ("scale" == k)
			{
				std::string szContent(v.get<std::string>());
				auto value = Foundation::splitString<float>(szContent, ",");
				scale = Vec3F(&value[0]);
			}
			else if ("rotation" == k)
			{
				std::string szContent(v.get<std::string>());
				auto value = Foundation::splitString<float>(szContent, ",");
				rotation = Vec3F(&value[0]);
			}
			else if ("model" == k)
			{
				szModel = v.get<std::string>();
			}
			else if ("visible" == k)
			{
				bVisible = v.get<bool>();
			}
		}
		GEngine->loadSkyEntity(pSky, szModel);
		if (bVisible)
		{
			pSky->show();
		}
		else
		{
			pSky->hide();
		}
		if (pSky->initialise())
		{
			mpSkyEntity = pSky;
			addEntity(pSky);
			WL_DECREASE(pSky);
		}
		else
		{
			WL_DELETE(pSky, Entity);
		}
		//TiXmlElement* pChildElement = pType->FirstChildElement();
		//while (pChildElement)
		//{
		//	std::map<std::string, std::string> SkyInfo;
		//	TiXmlAttribute* Attribute = pChildElement->FirstAttribute();  //��ȡbrotherԪ�صĵ�һ������
		//	while (Attribute != nullptr)
		//	{
		//		SkyInfo[Attribute->Name()] = Attribute->Value();            //��ȡAttribute���Ե�ֵ
		//		Attribute = Attribute->Next();
		//	}
		//	CSkyEntity* pSky = GEngine->createEntity<CSkyEntity>(EntityType::Sky);
		//	GEngine->loadSkyEntity(pSky, SkyInfo["Model"]);
		//	auto&& strVec = Foundation::split(SkyInfo["Position"], ",");
		//	if (strVec.size() > 0)
		//	{
		//		std::vector<float> vec;
		//		vec.reserve(strVec.size());
		//		for (auto& item : strVec)
		//		{
		//			vec.push_back(std::stof(item));
		//		}
		//		pSky->setPosition(Vec3F(&vec.front()));
		//	}
		//	if (1 == std::stoi(SkyInfo["Visible"]))
		//	{
		//		pSky->show();
		//	}
		//	else
		//	{
		//		pSky->hide();
		//	}
		//	if (pSky->initialise())
		//	{
		//		mpSkyEntity = pSky;
		//		pChildElement = pChildElement->NextSiblingElement();
		//		addEntity(pSky);
		//		WL_DECREASE(pSky);
		//	}
		//	else
		//	{
		//		WL_DELETE(pSky, Entity);
		//	}
		//}
	}


	void CScene::initSkyBox(const json::reference& value, WindowConfig* config)
	{
		//TiXmlElement* pChildElement = pType->FirstChildElement();
		//while (pChildElement)
		//{
		//	std::map<std::string, std::string> SkyInfo;
		//	TiXmlAttribute* Attribute = pChildElement->FirstAttribute();  //��ȡbrotherԪ�صĵ�һ������
		//	while (Attribute != nullptr)
		//	{
		//		SkyInfo[Attribute->Name()] = Attribute->Value();            //��ȡAttribute���Ե�ֵ
		//		Attribute = Attribute->Next();
		//	}
		//	CSkyBoxEntity* pSkyBox = GEngine->createEntity<CSkyBoxEntity>(EntityType::SkyBox);
		//	GEngine->loadSkyBoxEntity(pSkyBox, SkyInfo["Model"]);
		//	if (pSkyBox->initialise())
		//	{
		//		if (1 == std::stoi(SkyInfo["Visible"]))
		//		{
		//			pSkyBox->show();
		//		}
		//		else
		//		{
		//			pSkyBox->hide();
		//		}
		//		mpSkyBoxEntity = pSkyBox;
		//		pChildElement = pChildElement->NextSiblingElement();
		//		addEntity(pSkyBox);
		//		WL_DECREASE(pSkyBox);
		//	}
		//	else
		//	{
		//		//SafeDelete(pSkyBox)
		//		WL_DELETE(pSkyBox, Entity);
		//	}
		//}
	}

	void CScene::initBornIn(const json::reference& value, WindowConfig* config)
	{
		for (const auto& [k, v] : value.items())
		{
			if ("position" == k)
			{
				std::string szContent(v.get<std::string>());
				auto value = Foundation::splitString<float>(szContent, ",");
				mBornPos = value.data();
			}		
		}
	}

	void CScene::initTerrain(const json::reference& value, WindowConfig* config)
	{
		bool bVisible = false;
		CTerrainEntity* pTerrain = GEngine->createEntity<CTerrainEntity>(EntityType::Terrain);
		std::string sizeInfo = "";
		int buildType = 0;
		for (const auto& [k, v] : value.items())
		{
			if ("chunk" == k)
			{
				for (const auto& obj : v)
				{
					if (obj.contains("entities"))
					{
						initEntities(obj["entities"], config);
					}
				}
			}
			else if ("tiles" == k)
			{
				initChunks(v, config);
			}
			else if ("position" == k)
			{
				std::string szContent(v.get<std::string>());
				auto value = Foundation::splitString<float>(szContent, ",");
				pTerrain->setPosition(Vec3F(&value[0]));
			}
			else if ("builder" == k)
			{
				buildType = v.get<int>();
			}
			else if ("size" == k)
			{
				sizeInfo = v.get<std::string>();
			}
		}
		if (GEngine->loadTerrainEntity(pTerrain, buildType, sizeInfo))
		{
			mpTerrainEntity = pTerrain;
			addEntity(pTerrain);
		}
		WL_DECREASE(pTerrain);
	}


	void CScene::initGui(const json::reference& value, WindowConfig* config)
	{
		std::string szModel;
		bool bVisible = false;
		for (const auto& [k, v] : value.items())
		{
			if ("children" == k)
			{
				for (const auto& obj : v)
				{
					if (obj.contains("script"))
					{
						std::string szContent(obj["script"]);
						GEngine->doFile(szContent, "", 1);
						if (lua_istable(GEngine->getLuaState(), -1))
						{
							lua_pushnumber(GEngine->getLuaState(), 0);
							lua_gettable(GEngine->getLuaState(), -2);
							if (lua_isuserdata(GEngine->getLuaState(), -1))
							{
								CGuiEntity** ppobjct = (CGuiEntity**)lua_touserdata(GEngine->getLuaState(), -1);
								CGuiEntity* pGui = (*ppobjct);
								addEntity(pGui);
								WL_DECREASE(pGui);
							}
						}
					}
				}
			}
		}
	}

	void CScene::initOcean(const json& value, CEntity* pEntity)
	{
		auto pOcean = static_cast<COceanEntity*>(pEntity);
		if (value.contains("name"))
		{
			pOcean->setEntityName(value["name"]);
		}
		if (value.contains("model"))
		{
			GEngine->loadOceanEntity(pOcean, value["model"]);
			if (pOcean->initialise())
			{
				if (value.contains("position"))
				{
					std::string szContent(value["position"]);
					auto value = Foundation::splitString<float>(szContent, ",");
					pOcean->setPosition(Vec3F(&value[0]));
				}
				if (value.contains("visible"))
				{
					if (value["visible"])
					{
						pOcean->show();
					}
					else
					{
						pOcean->hide();
					}
				}
				mpOceanEntity = pOcean;
				addEntity(pOcean);
				WL_DECREASE(pOcean);
			}
			else
			{
				WL_DECREASE(pEntity);
			}
		}

	}

	void CScene::initActor(const json& value, CEntity* pEntity)
	{
		auto pActor = static_cast<CActorEntity*>(pEntity);
		if (value.contains("name"))
		{
			pActor->setEntityName(value["name"]);
		}
		if (value.contains("model"))
		{
			GEngine->loadActorEntity(pActor, value["model"]);
		}
		if (value.contains("visible"))
		{
			if (value["visible"])
			{
				pActor->show();
			}
			else
			{
				pActor->hide();
			}
		}
		if (value.contains("position"))
		{
			std::string szContent(value["position"]);
			auto valueArray = Foundation::splitString<float>(szContent, ",");
			pActor->setPosition(Vec3F(valueArray.data()));
		}
		if (value.contains("scale"))
		{
			std::string szContent(value["scale"]);
			auto valueArray = Foundation::splitString<float>(szContent, ",");
			pActor->setScale(Vec3F(valueArray.data()));
		}
		if (value.contains("rotation"))
		{
			std::string szContent(value["rotation"]);
			auto valueArray = Foundation::splitString<float>(szContent, ",");
			pActor->setRotation(Vec3F(valueArray.data()));
		}
		addEntity(pActor);
		WL_DECREASE(pActor);
	}

	void CScene::checkVoxleChunk()
	{
		auto& pos = mpMainCamera->getPosition();
		
	}

}