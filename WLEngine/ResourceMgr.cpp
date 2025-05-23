#include <fstream>
#include "ResourceMgr.h"
#include "WLEngine.h"
#include "GraphicHead.h"
#include "jsonHeader.h"

namespace WL
{
	struct SResourceFileHeader
	{
		unsigned int magic_number;
		int version;
		int nobjects;
	};

	struct SFaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	CResourceMgr::CResourceMgr()
	{
	}

	CResourceMgr::~CResourceMgr()
	{

	}


	bool CResourceMgr::loadMaterial(CResource* pResourceBase, const std::string& strFile)
	{
		try
		{
			static std::vector<std::string>	textureSampler = {
			{"textureSampler0"},
			{"textureSampler1"},
			{"textureSampler2"},
			{"textureSampler3"},
			{"textureSampler4"},
			{"textureSampler5"},
			{"textureSampler6"},
			{"textureSampler7"},
			};

			CMaterial* pMaterial = dynamic_cast<CMaterial*>(pResourceBase);
			lua_State* pLua = mpLua;

			if (!GEngine->doFile(pLua, strFile.c_str()))
			{
				throw false;
			}

			int iTop = lua_gettop(pLua);
			INT32 DepthTest = CScriptEngine::getTableItem<UINT32>(pLua, "material", "ZDepth");
			if (-1 != DepthTest)
			{
				pMaterial->mDepth = DepthTest;
			}
			INT32 DepthCompare = CScriptEngine::getTableItem<UINT32>(pLua, "material", "ZCompare");
			if (-1 != DepthCompare)
			{
				pMaterial->mDepthCompare = DepthCompare;
			}
			INT32 DepthWrite = CScriptEngine::getTableItem<UINT32>(pLua, "material", "ZWrite");
			if (-1 != DepthWrite)
			{
				pMaterial->mDepthWrite = DepthWrite;
			}
			INT32 Stencil = CScriptEngine::getTableItem<UINT32>(pLua, "material", "Stencil");
			if (-1 != Stencil)
			{
				pMaterial->mStencil = Stencil;
			}
			INT32 FillMode = CScriptEngine::getTableItem<UINT32>(pLua, "material", "FillMode");
			if (-1 != FillMode)
			{
				pMaterial->mFillMode = FillMode;
			}
			INT32 CullMode = CScriptEngine::getTableItem<UINT32>(pLua, "material", "CullMode");
			if (-1 != CullMode)
			{
				pMaterial->mCullMode = CullMode;
			}
			INT32 Transparent = CScriptEngine::getTableItem<UINT32>(pLua, "material", "Transparent");
			if (-1 != Transparent)
			{
				pMaterial->mTransparent = Transparent;
			}
			INT32 Shadow = CScriptEngine::getTableItem<UINT32>(pLua, "material", "Shadow");
			if (-1 != Shadow)
			{
				pMaterial->mShadow = Shadow;
			}
			INT32 Topology = CScriptEngine::getTableItem<UINT32>(pLua, "material", "Topology");
			if (-1 != Topology)
			{
				pMaterial->mTopology = Topology;
			}
			INT32 EnableLight = CScriptEngine::getTableItem<UINT32>(pLua, "material", "EnableLight");
			if (-1 != EnableLight)
			{
				pMaterial->mEnableLight = EnableLight;
			}

			auto passInfo = [&](const char* szSky, const char* pName)->std::string
			{
				int topx = lua_gettop(pLua);
				std::string szResult = "";
				lua_pushstring(pLua, szSky);
				lua_gettable(pLua, -2);
				lua_pushstring(pLua, pName);
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					szResult = lua_tostring(pLua, -1);
				}
				lua_settop(pLua, topx);
				return szResult;
			};

			std::string strVSVersion = "";
			std::string strHSVersion = "";
			std::string strDSVersion = "";
			std::string strPSVersion = "";
			std::string strGSVersion = "";
			std::string strCSVersion = "";

			std::string strMaterialVSVersion = "";
			std::string strMaterialHSVersion = "";
			std::string strMaterialDSVersion = "";
			std::string strMaterialPSVersion = "";
			std::string strMaterialGSVersion = "";
			std::string strMaterialCSVersion = "";

			std::string strVSMain = "";
			std::string strHSMain = "";
			std::string strDSMain = "";
			std::string strPSMain = "";
			std::string strGSMain = "";
			std::string strCSMain = "";

			std::string strVS = "";
			std::string strHS = "";
			std::string strDS = "";
			std::string strPS = "";
			std::string strGS = "";
			std::string strCS = "";

			strMaterialVSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "vsVersion");
			strMaterialHSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "hsVersion");
			strMaterialDSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "dsVersion");
			strMaterialPSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "psVersion");
			strMaterialGSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "gsVersion");
			strMaterialCSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "csVersion");

			INT32 nNum = CScriptEngine::getTableItem<INT32>(pLua, "material", "elementItemCount");
			INT32 nNumOne = CScriptEngine::getTableItem<INT32>(pLua, "material", "elementItemCountOne");
			std::vector<INPUT_ELEMENT_DESC> input_element;
			std::vector<INPUT_ELEMENT_DESC> input_elementOne;
			std::vector<std::vector<INPUT_ELEMENT_DESC>> allInputElement;
			if (nNum != -1)
			{
				if (nNum > 0)
				{
					input_element.resize(nNum);
					for (INT32 i = 0; i < nNum; ++i)
					{
						int itop = lua_gettop(pLua);
						lua_getglobal(pLua, "inputElementLayout");
						lua_pushnumber(pLua, i + 1);
						lua_call(pLua, 1, 7);
						input_element[i].SemanticName = lua_tostring(pLua, -7);
						input_element[i].SemanticIndex = (UINT)lua_tointeger(pLua, -6);
						input_element[i].Format = (DXGI_FORMAT)lua_tointeger(pLua, -5);
						input_element[i].InputSlot = (UINT)lua_tointeger(pLua, -4);
						input_element[i].AlignedByteOffset = (UINT)lua_tointeger(pLua, -3);
						input_element[i].InputSlotClass = (D3D11_INPUT_CLASSIFICATION)lua_tointeger(pLua, -2);
						input_element[i].InstanceDataStepRate = (UINT)lua_tointeger(pLua, -1);
						lua_settop(pLua, itop);
					}
					allInputElement.push_back(input_element);
				}
			}
			if (nNumOne > 0)
			{
				input_elementOne.resize(nNumOne);
				for (INT32 i = 0; i < nNumOne; ++i)
				{
					int itop = lua_gettop(pLua);
					lua_getglobal(pLua, "inputElementLayoutOne");
					lua_pushnumber(pLua, i + 1);
					lua_call(pLua, 1, 7);
					input_elementOne[i].SemanticName = lua_tostring(pLua, -7);
					input_elementOne[i].SemanticIndex = (UINT)lua_tointeger(pLua, -6);
					input_elementOne[i].Format = (DXGI_FORMAT)lua_tointeger(pLua, -5);
					input_elementOne[i].InputSlot = (UINT)lua_tointeger(pLua, -4);
					input_elementOne[i].AlignedByteOffset = (UINT)lua_tointeger(pLua, -3);
					input_elementOne[i].InputSlotClass = (D3D11_INPUT_CLASSIFICATION)lua_tointeger(pLua, -2);
					input_elementOne[i].InstanceDataStepRate = (UINT)lua_tointeger(pLua, -1);
					lua_settop(pLua, itop);
				}
				allInputElement.push_back(input_elementOne);
			}
			bool bResult = true;
			auto createShader = [&](int nKey, int inputDesc)->CShader*
			{
				if (strVSVersion.length() == 0)
				{
					strVSVersion = strMaterialVSVersion;
				}
				if (strDSVersion.length() == 0)
				{
					strDSVersion = strMaterialDSVersion;
				}
				if (strHSVersion.length() == 0)
				{
					strHSVersion = strMaterialHSVersion;
				}
				if (strGSVersion.length() == 0)
				{
					strGSVersion = strMaterialGSVersion;
				}
				if (strPSVersion.length() == 0)
				{
					strPSVersion = strMaterialPSVersion;
				}
				if (strCSVersion.length() == 0)
				{
					strCSVersion = strMaterialCSVersion;
				}
				const std::string& szShaderPath = strVS + strHS + strDS + strPS + strCS + strGS;
				CResource* pResource = findResource(szShaderPath);
				CShader* pShader = nullptr;
				if (szShaderPath.length() > 2)
				{
					if (pResource == nullptr)
					{
						auto& inputElement = allInputElement[inputDesc];
						pShader = dynamic_cast<CShader*>(createResource(Shader));
						pShader->setDevice(GEngine->getDevice());
						pShader->setResourceName(szShaderPath);
						if (strVS.length() > 2)
						{
							bResult = pShader->createVextexShader(strVS, strVSMain, strVSVersion, inputElement);
						}
						if (bResult && strPS.length() > 0)
						{
							pShader->createPixelShader(strPS, strPSMain, strPSVersion);
						}
						if (bResult && strHS.length() > 0)
						{
							bResult = pShader->createHullShader(strHS, strHSMain, strHSVersion);
						}
						if (bResult && strDS.length() > 0)
						{
							bResult = pShader->createDomainShader(strDS, strDSMain, strDSVersion);
						}
						if (bResult && strGS.length() > 0)
						{
							bResult = pShader->createGeometryShader(strGS, strGSMain, strGSVersion);
						}
						if (bResult && strCS.length() > 0)
						{
							bResult = pShader->createComputeShader(strCS, strCSMain, strCSVersion);
						}
						//	auto matrices = CRendererEngine::getSinglePtr()->getTransposeMatrices();
						//	pShader->addVSParam("", matrices.size() * sizeof(Matrix44), 0, 0);
					}
					else
					{
						pShader = dynamic_cast<CShader*>(pResource);
						pShader->addRef();
					}

					pShader->setZBufferWrite(pMaterial->mDepth);
					pShader->setZbufferCompare(pMaterial->mDepthCompare);
					pShader->setFillMode(pMaterial->mFillMode);
					pShader->setCullMode(pMaterial->mCullMode);
					pShader->setAlpha(pMaterial->mTransparent);
					pShader->setShadow(pMaterial->mShadow);
					pShader->setTopology(pMaterial->mTopology);
					pShader->setEnableLight(pMaterial->mEnableLight);

					_addVSConstBuffer(pShader, nKey);
					_addTextureConstBuffer(pShader, nKey);
					_addTypeBuffer(pShader, nKey);
					_addStructBuffer(pShader, nKey);
					_addHSConstBuffer(pShader, nKey);
					_addDSConstBuffer(pShader, nKey);
					_addPSConstBuffer(pShader, nKey);
					_addGSConstBuffer(pShader, nKey);
					_addCSConstBuffer(pShader, nKey);
					
					_addTextureBuffer(pShader, pMaterial, nKey);
					pMaterial->mpShaders.push_back(pShader);
				}
				return pShader;
			};
			lua_getglobal(pLua, "material");
			lua_pushstring(pLua, "lightShader");
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				static std::vector<std::string> shaderLight =
				{
					"No",
					"Ambient",
					"Direction",
					"Point",
				};

				for (const auto& item : shaderLight)
				{
					strVSVersion = passInfo(item.c_str(), "vsVersion");
					strHSVersion = passInfo(item.c_str(), "hsVersion");
					strDSVersion = passInfo(item.c_str(), "dsVersion");
					strPSVersion = passInfo(item.c_str(), "psVersion");
					strGSVersion = passInfo(item.c_str(), "gsVersion");
					strCSVersion = passInfo(item.c_str(), "csVersion");


					strVS = passInfo(item.c_str(), "vs");
					strPS = passInfo(item.c_str(), "ps");
					strHS = passInfo(item.c_str(), "hs");
					strDS = passInfo(item.c_str(), "ds");
					strGS = passInfo(item.c_str(), "gs");
					strCS = passInfo(item.c_str(), "cs");

					strVSMain = passInfo(item.c_str(), "vsMain");
					strPSMain = passInfo(item.c_str(), "psMain");
					strHSMain = passInfo(item.c_str(), "hsMain");
					strDSMain = passInfo(item.c_str(), "dsMain");
					strGSMain = passInfo(item.c_str(), "gsMain");
					strCSMain = passInfo(item.c_str(), "csMain");

					createShader(0, 0);
				}
			}


			UINT32 nPixeConstSize = 0;

			std::string strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", "AmbientColor");
			if (strContent.length() > 0)
			{
				auto split = Foundation::splitString<float>(strContent, ",");
				nPixeConstSize += sizeof(Vec4F);
				memcpy(&pMaterial->mAmbientColor.x, &split[0], sizeof(pMaterial->mAmbientColor));
			}

			strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", "DiffuseColor");
			if (strContent.length() > 0)
			{
				auto split = Foundation::splitString<float>(strContent, ",");
				nPixeConstSize += sizeof(Vec4F);
				memcpy(&pMaterial->mDiffuseColor.x, &split[0], sizeof(pMaterial->mDiffuseColor));
			}

			strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", "SpecularColor");
			if (strContent.length() > 0)
			{
				auto split = Foundation::splitString<float>(strContent, ",");
				nPixeConstSize += sizeof(Vec4F);
				memcpy(&pMaterial->mSpecularColor.x, &split[0], sizeof(pMaterial->mSpecularColor));
			}

			std::stringstream stream;
			std::string textureName = "";
			int t = lua_gettop(pLua);
			bool bCreteSmaplerDesc = false;
			for (INT32 i = 0; i < (INT32)textureSampler.size(); ++i)
			{
				SSamplerDesc samplerDesc;
				stream.str("");
				stream << i;
				textureName = "textureSampler" + stream.str();

				lua_settop(pLua, t);
				lua_getglobal(pLua, textureName.c_str());
				if (!lua_isnil(pLua, -1))
				{
					samplerDesc.Filter = (FILTER)CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "Filter");
					samplerDesc.AddressU = (TEXTURE_ADDRESS_MODE)CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "AddressU");
					samplerDesc.AddressV = (TEXTURE_ADDRESS_MODE)CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "AddressV");
					samplerDesc.AddressW = (TEXTURE_ADDRESS_MODE)CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "AddressW");
					samplerDesc.MipLODBias = CScriptEngine::getTableItem<float>(pLua, textureSampler[i].c_str(), "MipLODBias");
					samplerDesc.MaxAnisotropy = CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "MaxAnisotropy");
					samplerDesc.ComparisonFunc = (COMPARISON_FUNC)CScriptEngine::getTableItem<INT32>(pLua, textureSampler[i].c_str(), "ComparisonFunc");
					std::string color = CScriptEngine::getTableItem<const char*>(pLua, textureSampler[i].c_str(), "BorderColor");
					auto strList = Foundation::splitString<float>(color, ",");
					if (strList.size() > 0)
					{
						memcpy(samplerDesc.BorderColor, strList.data(), sizeof(float) * strList.size());
					}
					samplerDesc.MinLOD = CScriptEngine::getTableItem<float>(pLua, textureSampler[i].c_str(), "MinLOD");
					samplerDesc.MaxLOD = 1 >= CScriptEngine::getTableItem<float>(pLua, textureSampler[i].c_str(), "MaxLOD") ? D3D11_FLOAT32_MAX : CScriptEngine::getTableItem<float>(pLua, textureSampler[i].c_str(), "MaxLOD");
					
					pMaterial->initialiseSampler(&samplerDesc);
				}
				else
				{
					break;
				}
			}
			lua_settop(pLua, t);

			lua_getglobal(pLua, "material");
			lua_pushstring(pLua, "pass");
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				int nPass = (int)luaL_len(pLua, -1);
				auto passInfo = [&](int key, const char* pName)->std::string
				{
					int topx = lua_gettop(pLua);
					std::string szResult = "";
					lua_pushnumber(pLua, key);
					lua_gettable(pLua, -2);
					lua_pushstring(pLua, pName);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						szResult = lua_tostring(pLua, -1);
					}
					lua_settop(pLua, topx);
					return szResult;
				};
				for (int i = 0; i < nPass; ++i)
				{
					strVSVersion = std::move(passInfo(i + 1, "vsVersion"));
					strHSVersion = std::move(passInfo(i + 1, "hsVersion"));
					strDSVersion = std::move(passInfo(i + 1, "dsVersion"));
					strPSVersion = std::move(passInfo(i + 1, "psVersion"));
					strGSVersion = std::move(passInfo(i + 1, "gsVersion"));
					strCSVersion = std::move(passInfo(i + 1, "csVersion"));

					strVS = std::move(passInfo(i + 1, "vs"));
					strHS = std::move(passInfo(i + 1, "hs"));
					strDS = std::move(passInfo(i + 1, "ds"));
					strPS = std::move(passInfo(i + 1, "ps"));
					strGS = std::move(passInfo(i + 1, "gs"));
					strCS = std::move(passInfo(i + 1, "cs"));

					strVSMain = std::move(passInfo(i + 1, "vsMain"));
					strHSMain = std::move(passInfo(i + 1, "hsMain"));
					strDSMain = std::move(passInfo(i + 1, "dsMain"));
					strPSMain = std::move(passInfo(i + 1, "psMain"));
					strGSMain = std::move(passInfo(i + 1, "gsMain"));
					strCSMain = std::move(passInfo(i + 1, "csMain"));

					std::string szIndex = std::move(passInfo(i + 1, "InputDesc"));
					int nIndex = 0;
					if (!szIndex.empty())
					{
						nIndex = std::stoi(szIndex);
					}

					createShader(i + 1, nIndex);
				}
			}
			else
			{
				strVS = CScriptEngine::getTableItem<const char*>(pLua, "material", "vs");
				strHS = CScriptEngine::getTableItem<const char*>(pLua, "material", "hs");
				strDS = CScriptEngine::getTableItem<const char*>(pLua, "material", "ds");
				strPS = CScriptEngine::getTableItem<const char*>(pLua, "material", "ps");
				strGS = CScriptEngine::getTableItem<const char*>(pLua, "material", "gs");
				strCS = CScriptEngine::getTableItem<const char*>(pLua, "material", "cs");

				strVSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "vsVersion");

				strHSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "hsVersion");

				strDSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "dsVersion");

				strPSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "psVersion");

				strGSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "gsVersion");
				
				strCSVersion = CScriptEngine::getTableItem<const char*>(pLua, "material", "csVersion");


				strVSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "vsMain");

				strHSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "hsMain");

				strDSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "dsMain");

				strPSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "psMain");

				strGSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "gsMain");
				
				strCSMain = CScriptEngine::getTableItem<const char*>(pLua, "material", "csMain");

				createShader(0, 0);
			}
#if 0
			for (INT32 i = 0; i < (int)textureSampler.size(); ++i)
			{
				stream.str("");
				stream << i;
				textureName = "texture" + stream.str();
				stream.str("");
				strContent = CScriptEngine::getTableItem<const char*>(pLua, "material", textureName.c_str());
				if (strContent.length() > 0)
				{
					auto strList = foundation::splitString<std::string>(strContent, " ");
					CResource* pResource = findResource(strList[0]);

					if ("Target" == strList[0] || ("Dynamic" == strList[0]))
					{
						pMaterial->mpTextures.push_back(nullptr);
						continue;
					}
					CTexture* pTexture = nullptr;
					if (pResource == nullptr)
					{
						pTexture = dynamic_cast<CTexture*>(createResource(strList[0], Texture));
						if (strList.size() > 1)
						{
							int index = foundation::stringToType<int>(strList[1]);
							pTexture->setSampleState(pMaterial->getSample(index));
						}
						else
						{
							pTexture->setSampleState(pMaterial->getSample(0));
						}
						// Create a texture sampler state description.
						pTexture->initialiseShaderResourceView();

						pMaterial->mpTextures.push_back(pTexture);
					}
					else
					{
						pTexture = dynamic_cast<CTexture*>(pResource);
						pMaterial->mpTextures.push_back(pTexture);
						pResource->addRef();
					}
				}
				else
				{
					break;
				}
			}
#endif
			lua_settop(pLua, iTop);
			return bResult;
		}
		catch (...)
		{
			return false;
		}
		return true;
	}

	bool CResourceMgr::loadResource(CResource* pResourceBase, const std::string& strFile)
	{
		bool bLoadResoult = false;
		if (pResourceBase != nullptr)
		{
			pResourceBase->setResourceName(strFile);
			switch (pResourceBase->getType())
			{
			case Texture:
				bLoadResoult = loadTexture(pResourceBase, strFile);
				break;
			case Shader:
				bLoadResoult = loadShader(pResourceBase, strFile);
				break;
			case Mesh:
				bLoadResoult = loadMesh(pResourceBase, strFile);
				break;
			case Model:
				bLoadResoult = loadModel(pResourceBase, strFile);
				break;
			case Material:
				bLoadResoult = loadMaterial(pResourceBase, strFile);
				break;
			case Wnd:
				bLoadResoult = loadGui(pResourceBase, strFile);
				break;
			case Font:
				bLoadResoult = loadFont(pResourceBase, strFile);
				break;
			}
		}
		return bLoadResoult;
	}

	bool CResourceMgr::loadTexture(CResource* pResourceBase, const std::string& strFile)
	{
		return pResourceBase->initialise();
	}

	bool CResourceMgr::loadShader(CResource* pResourceBase, const std::string& strFile)
	{
		return true;
	}

	bool CResourceMgr::loadFont(CResource* pResourceBase, const std::string& strFile)
	{
		pResourceBase->initialise();
		return true;
	}

	bool CResourceMgr::loadGui(CResource* pResourceBase, const std::string& strFile)
	{
		//auto readJsonFile = [](const std::string& szName)->std::string
		//	{
		//		std::string szContent = "";
		//		std::ifstream file(szName.c_str()); // 打开文件
		//		if (!file)
		//		{
		//			return szName;
		//		}
		//		// 使用istreambuf_iterator来读取整个文件内容
		//		std::string content(std::istreambuf_iterator<char>(file), {});
		//		file.close(); // 关闭文件
		//		szContent = std::move(content);
		//		return szContent;
		//	};
		auto pGui = static_cast<CUIWnd*>(pResourceBase);
		std::string content(Foundation::readJsonFile(strFile));
		auto js = json::parse(content.c_str());
		auto config = GEngine->getWindowConfigPtr();
		auto item = js.items().begin();
		
		if (item.key() == "Wnd")
		{
			auto& value = item.value();
			if (value.contains("position"))
			{
				std::string szContent(value["position"]);
				auto value = Foundation::splitString<float>(szContent, ",");
				pGui->setPosition(Vec3F(&value[0]));
			}
			if (value.contains("size"))
			{
				std::string szContent(value["size"]);
				auto value = Foundation::splitString<float>(szContent, ",");
				pGui->setSize(Vec2F(&value[0]));
			}
			if (value.contains("rotation"))
			{
				std::string szContent(value["rotation"]);
			}
			if (value.contains("texture"))
			{
				std::string szContent(value["texture"]);
				pGui->setTexture(szContent);
			}
			if (value.contains("children"))
			{
				for (const auto& child : value["children"])
				{
					if (child.contains("type"))
					{
						WndType type = static_cast<WndType>(child["type"]);
						auto pChild = GEngine->createUI(type);

						if (child.contains("id"))
						{
							UINT32 id = static_cast<UINT32>(child["id"]);
							pChild->setId(id);
						}
						if (child.contains("position"))
						{
							std::string szContent(child["position"]);
							auto value = Foundation::splitString<float>(szContent, ",");
							pChild->setPosition(Vec3F(&value[0]));
						}
						if (child.contains("size"))
						{
							std::string szContent(child["size"]);
							auto value = Foundation::splitString<float>(szContent, ",");
							pChild->setSize(Vec2F(&value[0]));
						}
						if (child.contains("rotation"))
						{
							std::string szContent(child["rotation"]);
						}
						if (child.contains("texture"))
						{
							std::string szContent(child["texture"]);	
							pChild->setTexture(szContent);
						}
						if (child.contains("font"))
						{
							std::string szContent(child["font"]);
							pChild->setFont(szContent);
						}
						if (child.contains("uv"))
						{
							std::string szContent(child["uv"]);
							auto value = Foundation::splitString<float>(szContent, ",");
							pChild->mData.mArea[0].uv = Vec2F(value[0], value[1]);
							pChild->mData.mArea[1].uv = Vec2F(value[2], value[3]);
							pChild->mData.mArea[2].uv = Vec2F(value[4], value[5]);
							pChild->mData.mArea[3].uv = Vec2F(value[6], value[7]);
						}

						pGui->addChild(pChild);
						WL_DECREASE(pChild);					
					}
				}
			}

		}
		
		return true;
	}

	bool CResourceMgr::loadAnimation(CResource* pResourceBase, const std::string& strFile)
	{
		CModel* pModel = dynamic_cast<CModel*>(pResourceBase);
		if (Foundation::getFileExtension(strFile) == "oanim")
		{
			auto fReadDataType = [&](std::ifstream& fin)->std::string
			{
				UINT16 version;
				fin.read((char*)&version, sizeof(UINT16));
				if (0 == version)
				{
					return "";
				}
				char buffer[256];
				UINT8 len;

				fin.read((char*)&len, sizeof(UINT8));
				fin.read(buffer, len);
				buffer[len] = 0;
				return std::string(buffer);
			};

			auto fReadAnimationData = [&](std::ifstream& fin)
			{

			};
			if (nullptr == pModel->getComponent<CSkeletonAniComponent>())
			{
				pModel->addComponent<CSkeletonAniComponent>();
			}
			std::ifstream fin;

			fin.open(strFile.c_str(), std::ios::binary);

			// If it could not open the file then exit.
			if (fin.fail())
			{
				return false;
			}
			SResourceFileHeader header;
			fin.read((char*)&header, sizeof(header));

			auto buffer = fReadDataType(fin);
			if ("AnimationData" == buffer)
			{
				fReadAnimationData(fin);
			}
			fin.close();
		}
		return true;
	}

	bool CResourceMgr::initialize()
	{
		mpFactory = WL_NEW(CResourceFactory, Factory);
		mpLua = GEngine->createLuaState("CResourceMgr");
		return true;
	}

	void CResourceMgr::unInitialize()
	{
		mResourceMap.clear();
		mResourceMapByName.clear();
		mpLua = nullptr;
		WL_DELETE(mpFactory, Factory);
	}

	void CResourceMgr::releaseResource()
	{
		auto iter = mResourceMap.begin();
		auto iterEnd = mResourceMap.end();
		while (iter != iterEnd)
		{
			if ((iter->second)->getRefCount() == 1)
			{
				CResource* pResource = (iter->second);
				iter = mResourceMap.erase(iter);
				WL_DECREASE(pResource);
			}
			else
			{
				++iter;
			}
		}
	}

	void CResourceMgr::removeResource(CResource* pResource)
	{
		if (nullptr != pResource)
		{
			auto iter = mResourceMap.find(pResource->getUUID());
			auto iterByName = mResourceMapByName.find(pResource->getResourceName());
			if (iter != mResourceMap.end())
			{
				mResourceMap.erase(iter);
			}
			if (iterByName != mResourceMapByName.end())
			{
				mResourceMapByName.erase(iterByName);
			}
		}
	}

	bool CResourceMgr::loadModel(CResource* pResourceBase, const std::string& strFile)
	{
		bool bResult = false;
		CModel* pModel = dynamic_cast<CModel*>(pResourceBase);
		lua_State* pLua = mpLua;
		if (nullptr != pModel && GEngine->doFile(pLua, strFile))
		{
			int iTop = lua_gettop(pLua);
			do
			{
				lua_getglobal(pLua, "model");
				int topx = lua_gettop(pLua);
				lua_pushstring(pLua, "textures");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nCount = (int)luaL_len(pLua, -1);
					for (int i = 1; i <= nCount; ++i)
					{
						lua_pushnumber(pLua, i);
						lua_gettable(pLua, -2);
						if (!lua_isnil(pLua, -1))
						{
							const char* pStr = nullptr;
							lua_pushstring(pLua, "name");
							lua_gettable(pLua, -2);
							pStr = lua_tostring(pLua, -1);
							lua_pop(pLua, 1);
							if (nullptr != pStr)
							{
								pStr = nullptr;
							}

							lua_pushstring(pLua, "path");
							lua_gettable(pLua, -2);
							pStr = lua_tostring(pLua, -1);
							lua_pop(pLua, 1);
							if (nullptr != pStr)
							{
								pStr = nullptr;
							}
						}
					}
				}
				lua_settop(pLua, topx);
				lua_pushstring(pLua, "meshes");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nPass = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					auto passInfo = [&](int key, const char* pName)->std::string
					{
						std::string szResult = "";
						lua_pushnumber(pLua, key);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, pName);
						lua_gettable(pLua, -2);
						if (!lua_isnil(pLua, -1))
						{
							szResult = lua_tostring(pLua, -1);
						}
						lua_settop(pLua, topx);
						return szResult;
					};

					auto processMesh = [&](CMesh* pMesh, int i)
					{
						if (nullptr != pMesh)
						{
							auto strPosition = std::move(passInfo(i + 1, "position"));
							auto strScale = std::move(passInfo(i + 1, "scale"));
							auto strRotation = std::move(passInfo(i + 1, "rotation"));
							{
								auto split = Foundation::splitString<float>(strScale, ",");
								pMesh->setScale(Vec3F(&split[0]));
							}
							{
								auto split = Foundation::splitString<float>(strRotation, ",");
								pMesh->setRotation(Vec3F(&split[0]));
							}
							{
								auto split = Foundation::splitString<float>(strPosition, ",");
								pMesh->setPosition(Vec3F(&split[0]));
							}

							auto szMaterial = std::move(passInfo(i + 1, "material"));
							auto pMaterial = dynamic_cast<CMaterial*>(createResource(szMaterial, Material));
							if (nullptr != pMaterial)
							{
								pModel->setMaterial(pMaterial);
								SMeshNode info;
								for (int n = 0; n < CountArray(szTextureFlag); ++n)
								{
									auto strTexture = std::move(passInfo(i + 1, szTextureFlag[n].c_str()));
									if (!strTexture.empty())
									{
										auto strList = Foundation::splitString<std::string>(strTexture, " ");

										if ("textureArray" != szTextureFlag[n])
										{
											auto pTexture = dynamic_cast<CTexture*>(createResource(strList[0], Texture));
											if (nullptr != pTexture)
											{
												info.textures[n] = pTexture;
												pTexture->addRef();
												int index = 0;
												if (strList.size() > 1)
												{
													index = Foundation::stringToType<int>(strList[1]);
												}
												auto pSample = pMaterial->getSampler(index);
												if (nullptr != pSample)
												{
													pTexture->setSampleState(pSample);
												}
											}
										}
										else
										{
											int nTextureIndex = 4;
											for (int i = 0; i < (int)strList.size(); i += 2)
											{
												auto pTexture = dynamic_cast<CTexture*>(createResource(strList[i], Texture));
												if (nullptr != pTexture)
												{
													int index = Foundation::stringToType<int>(strList[i + 1]);
													auto pSample = pMaterial->getSampler(index);
													if (nullptr != pSample)
													{
														pTexture->setSampleState(pSample);
													}
													else
													{
														pSample = pMaterial->getSampler(0);
														if (nullptr != pSample)
														{
															pTexture->setSampleState(pSample);
														}
													}
													info.textureArray.addTexture(pTexture);
												}
											}
										}
									}
								}
								info.pMesh = pMesh;
								info.pMaterial = pMaterial;
								pModel->addMesh(info);
							}

							auto strAnimation = std::move(passInfo(i + 1, "animation"));
							if (strAnimation.length() > 0)
							{
								loadAnimation(pResourceBase, strAnimation);
							}
						}

					};
					for (int i = 0; i < nPass; ++i)
					{
						auto strPath = std::move(passInfo(i + 1, "path"));
						auto szName = Foundation::getFileExtension(strPath);

						if ("omod" != szName)
						{
							CMesh* pMesh = nullptr;
 							if ("BillBoard" == strPath)
							{
								auto pConfig = GEngine->getWindowConfigPtr();
								pMesh = createBillBoardMesh(pConfig->nWidth, pConfig->nHeight);
							}
							else
							{
								pMesh = dynamic_cast<CMesh*>(createResource(strPath, Mesh));
							}
							processMesh(pMesh, i);
						}
						else
						{
							auto&& meshes = _loadMesh_omod(pResourceBase, strPath);
							for (auto item : meshes)
							{
								processMesh(item, i);
							}
						}
					}
				}
				std::string strPosition = CScriptEngine::getTableItem<const char*>(pLua, "model", "position");
				std::string strScale = CScriptEngine::getTableItem<const char*>(pLua, "model", "scale");
				std::string strRotation = CScriptEngine::getTableItem<const char*>(pLua, "model", "rotation");
				{
					auto split = Foundation::splitString<float>(strScale, ",");
					pModel->setScale(Vec3F(&split[0]));
				}
				{
					auto split = Foundation::splitString<float>(strRotation, ",");
					pModel->setRotation(Vec3F(&split[0]));
				}
				{
					auto split = Foundation::splitString<float>(strPosition, ",");
					pModel->setPosition(Vec3F(&split[0]));
				}
				bResult = true;
			} while (false);
			lua_settop(pLua, iTop);
		}

		return bResult;
	}

	bool CResourceMgr::loadMesh(CResource* pResourceBase, const std::string& strFile)
	{
		auto pMesh = dynamic_cast<CMesh*>(pResourceBase);
		bool bResult = false;
		if (Foundation::getExterName(strFile) == "m")
		{
			bResult = _loadMesh_m(pMesh, strFile);
		}
		else if (Foundation::getExterName(strFile) == "obj")
		{
			bResult = _loadMesh_obj(pMesh, strFile);
		}
		else if (Foundation::getExterName(strFile) == "m2")
		{
			bResult = _loadMesh_mTwo(pMesh, strFile);
		}
		else if (Foundation::getExterName(strFile) == "fbx")
		{
			bResult = _loadMesh_fbx(pMesh, strFile);
		}
		else if (Foundation::getExterName(strFile) == "txt")
		{
			bResult = _loadMesh_txt(pMesh, strFile);
		}
		else if (Foundation::getExterName(strFile) == "sdkmesh")
		{
			bResult = _loadMesh_sdkmesh(pResourceBase, strFile);
		}
		if (bResult)
		{
			calculationBoundBox(pMesh);
		}
		return true;
	}

	CResource* CResourceMgr::createResource(ResourceType type) noexcept
	{
		try
		{
			CResource* pResource = mpFactory->createResource(type);
			if (pResource != nullptr)
			{
				UINT32 uuid = pResource->getUUID();
				mResourceMap[uuid] = pResource;
				return pResource;
			}
			else
			{
				throw(0);
			}
		}
		catch (...)
		{
			return nullptr;
		}
	}

	CResource* CResourceMgr::createResource(std::string fileName, ResourceType type) noexcept
	{
		std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
		if (fileName.length() > 0)
		{
			auto item = mResourceMapByName.find(fileName);
			if (item == mResourceMapByName.end())
			{
				CResource* pResource = createResource(type);
				bool bResult = loadResource(pResource, fileName);
				if (bResult)
				{
					pResource->setResourceName(fileName);
					mResourceMapByName[fileName] = pResource;
					UINT32 uuid = pResource->getUUID();
					mResourceMap[uuid] = pResource;
				}
				else
				{
					auto iter = mResourceMap.find(pResource->getUUID());
					if (iter != mResourceMap.end())
					{
						mResourceMap.erase(iter);
					}
					WL_DECREASE(pResource);
				}
				return pResource;
			}
			else
			{
				return (item->second);
			}
		}
		else
		{
			return createResource(type);
		}
	}

	CResource* CResourceMgr::findResource(std::string fileName)
	{
		transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);

		auto iter = mResourceMapByName.find(fileName);
		if (iter == mResourceMapByName.end())
		{
			return nullptr;
		}
		else
		{
			return iter->second;
		}
	}


	CActorEntity* CResourceMgr::createBillBoardEntity(const std::string& szName, int nWidth, int nHeight, float zDepth/*=1.0f*/)
	{
		CModel* pModel = createBillBoardModel(szName, nWidth, nHeight, zDepth);
		auto pEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);
		pEntity->addModel(pModel);
		auto pModelInstance = pEntity->getModelInstance();
		auto pMatInstance = pModelInstance->getMaterialInstance();
		pMatInstance->setRenderOrder(256);
		WL_DECREASE(pModel);
		return pEntity;
	}

	CModel* CResourceMgr::createBillBoardModel(const std::string& szName, int nWidth, int nHeight, float zDepth /*= 1.0f*/)
	{
		int nVecticeCount = 6;
		SVertexVT* pVB = WL_NEW_ARRAY(SVertexVT, nVecticeCount);
		if (nullptr == pVB)
		{
			return nullptr;
		}

		UINT32* pIB = WL_NEW_ARRAY(UINT32, nVecticeCount);
		if (nullptr == pIB)
		{
			return nullptr;
		}

		// Calculate the screen coordinates of the left side of the window.
		float left = -(float)nWidth / 2;

		// Calculate the screen coordinates of the right side of the window.
		float right = left + (float)nWidth;

		// Calculate the screen coordinates of the top of the window.
		float top = (float)nHeight / 2;

		// Calculate the screen coordinates of the bottom of the window.
		float bottom = top - (float)nHeight;


		// Load the vertex array with data.
		// First triangle.
		pVB[0].position = Vec3F(left, top, zDepth);  // Top left.
		pVB[0].uv = Vec2F(0.0f, 0.0f);

		pVB[1].position = Vec3F(right, bottom, zDepth);  // Bottom right.
		pVB[1].uv = Vec2F(1.0f, 1.0f);

		pVB[2].position = Vec3F(left, bottom, zDepth);  // Bottom left.
		pVB[2].uv = Vec2F(0.0f, 1.0f);

		// Second triangle.
		pVB[3].position = Vec3F(left, top, zDepth);  // Top left.
		pVB[3].uv = Vec2F(0.0f, 0.0f);

		pVB[4].position = Vec3F(right, top, zDepth);  // Top right.
		pVB[4].uv = Vec2F(1.0f, 0.0f);

		pVB[5].position = Vec3F(right, bottom, zDepth);  // Bottom right.
		pVB[5].uv = Vec2F(1.0f, 1.0f);

		// Load the index array with data.
		for (int i = 0; i < nVecticeCount; i++)
		{
			pIB[i] = i;
		}
		auto pEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);

		CModel* pModel = dynamic_cast<CModel*>(GEngine->createResource(Model));
		pModel->addMesh(szName, pVB, sizeof(SVertexVT) * 6, sizeof(SVertexVT), pIB, sizeof(UINT32) * 6, sizeof(UINT32));
		WL_DELETE_ARRAY(pVB);
		WL_DELETE_ARRAY(pIB);
		return pModel;
	}

	CMesh* CResourceMgr::createBillBoardMesh(int nWidth, int nHeight, float zDepth /*= 1.0f*/)
	{
		int nVecticeCount = 6;
		SVertexVT* pVB = WL_NEW_ARRAY(SVertexVT, nVecticeCount);
		if (nullptr == pVB)
		{
			return nullptr;
		}

		UINT32* pIB = WL_NEW_ARRAY(UINT32, nVecticeCount);
		if (nullptr == pIB)
		{
			return nullptr;
		}

		// Calculate the screen coordinates of the left side of the window.
		float left = -(float)nWidth / 2;

		// Calculate the screen coordinates of the right side of the window.
		float right = left + (float)nWidth;

		// Calculate the screen coordinates of the top of the window.
		float top = (float)nHeight / 2;

		// Calculate the screen coordinates of the bottom of the window.
		float bottom = top - (float)nHeight;


		// Load the vertex array with data.
		// First triangle.
		pVB[0].position = Vec3F(left, top, zDepth);  // Top left.
		pVB[0].uv = Vec2F(0.0f, 0.0f);

		pVB[1].position = Vec3F(right, bottom, zDepth);  // Bottom right.
		pVB[1].uv = Vec2F(1.0f, 1.0f);

		pVB[2].position = Vec3F(left, bottom, zDepth);  // Bottom left.
		pVB[2].uv = Vec2F(0.0f, 1.0f);

		// Second triangle.
		pVB[3].position = Vec3F(left, top, zDepth);  // Top left.
		pVB[3].uv = Vec2F(0.0f, 0.0f);

		pVB[4].position = Vec3F(right, top, zDepth);  // Top right.
		pVB[4].uv = Vec2F(1.0f, 0.0f);

		pVB[5].position = Vec3F(right, bottom, zDepth);  // Bottom right.
		pVB[5].uv = Vec2F(1.0f, 1.0f);

		// Load the index array with data.
		for (int i = 0; i < nVecticeCount; i++)
		{
			pIB[i] = i;
		}
		return createMesh(pVB, sizeof(SVertexVT) * 6, sizeof(SVertexVT), pIB, sizeof(UINT32) * 6, sizeof(UINT32));

	}

	CMesh* CResourceMgr::createMesh(void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize)
	{
		auto pMesh = dynamic_cast<CMesh*>(GEngine->createResource(Mesh));
		if (nullptr != pMesh)
		{
			GEngine->commitCommand(commandType::StopDraw);
			pMesh->fillVertexBuffer(pVertices, nVerticesCount, VertexSize);
			pMesh->fillIndexBuffer(pIndices, nIndicesCount, IndexSize);
			GEngine->commitCommand(commandType::RecoverDraw);
		}
		return pMesh;
	}

	void CResourceMgr::_addVSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "vsConstBuffer");
		lua_gettable(pLua, -2);

		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, INT32> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (lua_istable(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));

						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addVSValue(name, size, registerIndex, packoffset);

						constBufferMap[registerIndex] += size;
					}
				}
				for (auto item : constBufferMap)
				{
					std::ostringstream oss;
					oss << "vsConstBuffer" << item.first;
					pShader->addVSConstBuffer(oss.str().c_str(), item.second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "vsConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addVSConstBuffer(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addHSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "hsConstBuffer");
		lua_gettable(pLua, -2);
		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, INT32> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (lua_istable(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						lua_pop(pLua, 2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						pShader->addHSValue(name, size, registerIndex, packoffset);
						constBufferMap[registerIndex] += size;
					}
				}
				for (auto item : constBufferMap)
				{
					lua_pop(pLua, 1);
					std::ostringstream oss;
					oss << "hsConstBuffer" << item.first;
					pShader->addHSConstBuffer(oss.str().c_str(), item.second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "hsConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addHSConstBuffer(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addDSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "dsConstBuffer");
		lua_gettable(pLua, -2);
		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, INT32> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addDSValue(name, size, registerIndex, packoffset);
						constBufferMap[registerIndex] += size;

					}
				}
				for (auto item : constBufferMap)
				{
					std::ostringstream oss;
					oss << "dsConstBuffer" << item.first;
					pShader->addDSConstBuffer(oss.str().c_str(), item.second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "dsConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addDSConstBuffer(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addPSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "psConstBuffer");
		lua_gettable(pLua, -2);
		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				std::map<UINT32, INT32> constBufferMap;
				if (pShader->isEnableLight())
				{
					const auto& params = GEngine->getGlobalShaderParam(ShaderBufferType::PS);
					for (auto item : params)
					{
						constBufferMap[item.nRegister] += item.nSize;
						pShader->addPSValue(item.szName, item.nSize, item.nRegister, item.packOffset);
					}
				}
				INT32 nCount = Foundation::calculateTableLength(pLua);
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);

					if (lua_istable(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addPSValue(name, size, registerIndex, packoffset);
						constBufferMap[registerIndex] += size;
					}
				}
				for (auto item : constBufferMap)
				{
					std::ostringstream oss;
					oss << "psConstBuffer" << item.first;
					pShader->addPSConstBuffer(oss.str().c_str(), item.second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "psConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addPSConstBuffer(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addGSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "gsConstBuffer");
		lua_gettable(pLua, -2);
		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, INT32> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addGSValue(name, size, registerIndex, packoffset);
						constBufferMap[registerIndex] += size;
					}
				}
				for (auto item : constBufferMap)
				{
					std::ostringstream oss;
					oss << "gsConstBuffer" << item.first;
					pShader->addGSConstBuffer(oss.str().c_str(), item.second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "gsConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addGSConstBuffer(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addCSConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "csConstBuffer");
		lua_gettable(pLua, -2);

		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, INT32> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (lua_istable(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);


						lua_pushstring(pLua, "usage");
						lua_gettable(pLua, -2);
						INT32 nUsage = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "bindFlags");
						lua_gettable(pLua, -2);
						INT32 nBindFlags = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "cpuAccessFlags");
						lua_gettable(pLua, -2);
						INT32 nCpuAccessFlags = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex];
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addCSValue(name, size, registerIndex, packoffset, nUsage, nBindFlags,nCpuAccessFlags);

						constBufferMap[registerIndex] += size;
					}
				}
				//for (auto item : constBufferMap)
				//{
				//	std::ostringstream oss;
				//	oss << "csConstBuffer" << item.first;
				//	pShader->addCSConstBuffer(oss.str().c_str(), item.second, item.first, n);
				//}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addTextureConstBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "textureConstBuffer");
		lua_gettable(pLua, -2);

		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 2);

						//lua_pushstring(pLua, "packoffset");
						//lua_gettable(pLua, -2);
						//INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						//lua_pop(pLua, 2);
						pShader->addTextureConstBufferParam(name, size, registerIndex);
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "textureConstBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						//lua_pushnumber(pLua, i + 1);
						//lua_gettable(pLua, -2);
						//lua_pushstring(pLua, "packoffset");
						//lua_gettable(pLua, -2);
						//INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						//lua_settop(pLua, topx);
						pShader->addTextureConstBufferParam(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addTextureBuffer(CShader* pShader, CMaterial* pMaterial, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "textureBuffer");
		lua_gettable(pLua, -2);

		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				//std::map<UINT32, ImgFileInfo> textureMap;
				INT32 nCount = Foundation::calculateTableLength(pLua);
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						SImgFileInfo fileInfo;
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "textureType");
						lua_gettable(pLua, -2);
						INT32 nTextureType = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						fileInfo.nType = nTextureType;

						const char* pStr = nullptr;
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = "";
						pStr = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);
						if (nullptr != pStr)
						{
							name = pStr;
						}

						lua_pushstring(pLua, "path");
						lua_gettable(pLua, -2);
						pStr = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);
						if (nullptr != pStr)
						{
							fileInfo.szPath = pStr;
							pStr = nullptr;
						}

						lua_pushstring(pLua, "width");
						lua_gettable(pLua, -2);
						INT32 nWidth = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						fileInfo.nWidth = nWidth;

						lua_pushstring(pLua, "height");
						lua_gettable(pLua, -2);
						INT32 nHeight = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						fileInfo.nHeight = nHeight;

						lua_pushstring(pLua, "depth");
						lua_gettable(pLua, -2);
						INT32 nDepth = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						fileInfo.nDepth = nDepth;

						lua_pushstring(pLua, "format");
						lua_gettable(pLua, -2);
						INT32 nFormat = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						fileInfo.nFormat = nFormat;

						lua_pushstring(pLua, "usage");
						lua_gettable(pLua, -2);
						INT32 nUsage = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "bindFlags");
						lua_gettable(pLua, -2);
						INT32 nBindFlags = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "cpuAccessFlags");
						lua_gettable(pLua, -2);
						INT32 nCpuAccessFlags = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "shaderFlags");
						lua_gettable(pLua, -2);
						INT32 nShaderFlags = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "sampler");
						lua_gettable(pLua, -2);
						INT32 nSampler = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);
						//fileInfo.nFormat = nFormat;

						auto pSampler = pMaterial->getSampler(nSampler);
						if (pSampler != nullptr)
						{
							pShader->setSampler(nSampler, pSampler);
						}
						if (name.length() < 4)
						{
							lua_pushstring(pLua, "size");
							lua_gettable(pLua, -2);
							fileInfo.nSliceSize = static_cast<UINT32>(lua_tonumber(pLua, -1));
							lua_pop(pLua, 2);
						}
						else
						{
							lua_pop(pLua, 1);
						}
						if(fileInfo.szPath.length() > 4)
						{
							std::ifstream infile;
							infile.open(fileInfo.szPath.c_str(), std::ios::in);
							infile.seekg(0, std::ios::end);
							fileInfo.nFileSize = (UINT32)infile.tellg();
							infile.seekg(0, std::ios::beg);
							infile.close();
						}
						pShader->addTextureBufferValue(name, registerIndex, fileInfo, nShaderFlags, nUsage, nBindFlags, nCpuAccessFlags);
						//textureMap[registerIndex] = fileInfo;
					}
				}
				//for (auto item : textureMap)
				//{
				//	std::ostringstream oss;
				//	oss << "textureBuffer" << item.first;
				//	ImgFileInfo info;
				//	pShader->addTextureParam(oss.str().c_str(), item.first, item.second);
				//}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "textureBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "textureType");
						lua_gettable(pLua, -2);
						INT32 nTextureType = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "width");
						lua_gettable(pLua, -2);
						INT32 nWidth = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "height");
						lua_gettable(pLua, -2);
						INT32 nHeight = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "format");
						lua_gettable(pLua, -2);
						INT32 nFormat = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);

						//lua_pushnumber(pLua, i + 1);
						//lua_gettable(pLua, -2);
						//lua_pushstring(pLua, "packoffset");
						//lua_gettable(pLua, -2);
						//INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						//lua_settop(pLua, topx);
						//pShader->addTextureParam(name, registerIndex, ImgFileInfo(size, nFormat, nWidth, nHeight,nTextureType));
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	void CResourceMgr::_addTypeBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "typeBuffer");
		lua_gettable(pLua, -2);

		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 2);

						//lua_pushstring(pLua, "packoffset");
						//lua_gettable(pLua, -2);
						//INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						//lua_pop(pLua, 2);
						pShader->addTypeBufferParam(name, size, registerIndex);
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "typeBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						//lua_pushnumber(pLua, i + 1);
						//lua_gettable(pLua, -2);
						//lua_pushstring(pLua, "packoffset");
						//lua_gettable(pLua, -2);
						//INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						//lua_settop(pLua, topx);
						pShader->addTypeBufferParam(name, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}



	void CResourceMgr::_addStructBuffer(CShader* pShader, int nKey)
	{
		lua_State* pLua = mpLua;
		int t = lua_gettop(pLua);
		lua_getglobal(pLua, "material");
		lua_pushstring(pLua, "structBuffer");
		lua_gettable(pLua, -2);
		if (!lua_isnil(pLua, -1))
		{
			if (lua_istable(pLua, -1))
			{
				INT32 nCount = Foundation::calculateTableLength(pLua);
				std::map<UINT32, std::pair<INT32,INT32>> constBufferMap;
				for (INT32 i = 1; i < nCount; ++i)
				{
					lua_pushnumber(pLua, i);
					lua_gettable(pLua, -2);
					if (!lua_isnil(pLua, -1))
					{
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "structSize");
						lua_gettable(pLua, -2);
						INT32 structSize = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_pop(pLua, 1);

						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = constBufferMap[registerIndex].first;
						if (!lua_isnil(pLua, -1))
						{
							packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						}
						lua_pop(pLua, 2);
						pShader->addStructValue(name, size, structSize, registerIndex, packoffset);
						constBufferMap[registerIndex] = std::make_pair(structSize, size);

					}
				}
				for (auto item : constBufferMap)
				{
					std::ostringstream oss;
					oss << "structBuffer" << item.first;
					pShader->addStructedParam(oss.str().c_str(), (item.second).first, (item.second).second, item.first);
				}
			}
		}
		else
		{
			lua_settop(pLua, t);
			lua_pushnumber(pLua, nKey);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, "structBuffer");
				lua_gettable(pLua, -2);
				if (!lua_isnil(pLua, -1))
				{
					int nLength = (int)luaL_len(pLua, -1);
					int topx = lua_gettop(pLua);
					for (int i = 0; i < nLength; ++i)
					{
						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "name");
						lua_gettable(pLua, -2);
						std::string name = lua_tostring(pLua, -1);
						lua_settop(pLua, topx);

						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "size");
						lua_gettable(pLua, -2);
						INT32 size = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "registerIndex");
						lua_gettable(pLua, -2);
						INT32 registerIndex = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);


						lua_pushnumber(pLua, i + 1);
						lua_gettable(pLua, -2);
						lua_pushstring(pLua, "packoffset");
						lua_gettable(pLua, -2);
						INT32 packoffset = static_cast<INT32>(lua_tonumber(pLua, -1));
						lua_settop(pLua, topx);
						pShader->addStructedParam(name, size, size, registerIndex);
					}
				}
			}
		}
		lua_settop(pLua, t);
	}

	bool CResourceMgr::_loadMesh_obj(CMesh* pMesh, const std::string& strFile)
	{
		std::ifstream fin;
		char input = 0;
		// Open the model file.
		fin.open(strFile.c_str());

		// If it could not open the file then exit.
		if (fin.fail())
		{
			return false;
		}

		std::vector<SVertexFormatVTN> vertices;
		std::vector<UINT16> indices;

		// Read up to the value of vertex count.
		fin.get(input);
		int v = 0;
		int t = 0;
		int n = 0;
		std::vector<Vec3F>	position;
		std::vector<Vec2F>	uv;
		std::vector<Vec3F>	normal;
		std::vector<SFaceType> faces;
		while (!fin.eof())
		{
			if (input == 'v')
			{
				Vec3F v3;
				fin.get(input);

				// Read in the vertices.
				if (input == ' ')
				{
					fin >> v3.x >> v3.y >> v3.z;
					// Invert the Z vertex to change to left hand system.
					v3.z = v3.z * -1.0f;
					position.emplace_back(v3);
					++v;
				}

				// Read in the texture uv coordinates.
				if (input == 't')
				{
					Vec2F v2;
					fin >> v2.x >> v2.y;

					// Invert the V texture coordinates to left hand system.
					v2.y = 1.0f - v2.y;
					uv.emplace_back(v2);
					++t;
				}

				// Read in the normals.
				if (input == 'n')
				{
					Vec3F v3;
					fin >> v3.x >> v3.y >> v3.z;

					// Invert the Z normal to change to left hand system.
					v3.z = v3.z * -1.0f;
					normal.emplace_back(v3);
					++n;
				}
			}
			// Read in the faces.
			if (input == 'f')
			{
				fin.get(input);
				if (input == ' ')
				{
					SFaceType face;
					char input2;
					fin >> face.vIndex3 >> input2 >> face.tIndex3 >> input2 >> face.nIndex3
						>> face.vIndex2 >> input2 >> face.tIndex2 >> input2 >> face.nIndex2
						>> face.vIndex1 >> input2 >> face.tIndex1 >> input2 >> face.nIndex1;

					faces.emplace_back(face);
				}
			}

			// Read in the remainder of the line.
			while (input != '\n')
			{
				fin.get(input);
			}

			// Start reading the beginning of the next line.
			fin.get(input);
		}

		// Close the file.
		fin.close();
		int vIndex, tIndex, nIndex;
		int indicesCount = -1;
		for (int i = 0; i < (int)faces.size(); i++)
		{
			SVertexFormatVTN v;
			vIndex = faces[i].vIndex1 - 1;
			tIndex = faces[i].tIndex1 - 1;
			nIndex = faces[i].nIndex1 - 1;

			v.position = position[vIndex];
			v.uv = uv[tIndex];
			v.normal = normal[nIndex];

			vertices.emplace_back(v);
			indices.push_back(++indicesCount);

			vIndex = faces[i].vIndex2 - 1;
			tIndex = faces[i].tIndex2 - 1;
			nIndex = faces[i].nIndex2 - 1;

			v.position = position[vIndex];
			v.uv = uv[tIndex];
			v.normal = normal[nIndex];

			vertices.emplace_back(v);
			indices.push_back(++indicesCount);

			vIndex = faces[i].vIndex3 - 1;
			tIndex = faces[i].tIndex3 - 1;
			nIndex = faces[i].nIndex3 - 1;

			v.position = position[vIndex];
			v.uv = uv[tIndex];
			v.normal = normal[nIndex];

			vertices.emplace_back(v);
			indices.push_back(++indicesCount);
		}
		pMesh->fillVertexBuffer(&vertices[0], sizeof(SVertexFormatVTN) * vertices.size(), sizeof(SVertexFormatVTN));
		pMesh->fillIndexBuffer(&indices[0], sizeof(UINT16) * indices.size(), sizeof(UINT16));
		return true;
	}

	std::vector<CMesh*> CResourceMgr::_loadMesh_omod(CResource* pResource, const std::string& strFile)
	{
		std::vector<UINT16> indices;
		std::vector<SVertexFormatOMOD> vertices;
		std::vector<CMesh*> meshes;
		Vec3F minPos, maxPos;
		bool bCreateMesh = false;
		auto fReadDataType = [&](std::ifstream& fin)->std::string
		{
			UINT16 version;
			fin.read((char*)&version, sizeof(UINT16));
			if (0 == version)
			{
				return "";
			}
			char buffer[256];
			UINT8 len;

			fin.read((char*)&len, sizeof(UINT8));
			fin.read(buffer, len);
			buffer[len] = 0;
			return std::string(buffer);
		};
		auto fReadPatches = [&](std::ifstream& fin)
		{
			struct SSkinPatch
			{
				char data[96];
			};
			UINT32 num;
			fin.read((char*)&num, sizeof(UINT32));
			for (UINT32 i = 0; i < num; ++i)
			{

			}

		};

		auto fReadSkeletonData = [&](std::ifstream& fin)
		{

		};
		auto fReadMaterialData = [&](std::ifstream& fin)
		{
			enum ShaderParamType
			{
				SPT_FLOAT = 0,
				SPT_FLOAT2,
				SPT_FLOAT3,
				SPT_FLOAT4,
				SPT_FLOAT3X3,
				SPT_TEXTURE,
				SPT_COLOR,
				SPT_FLOAT4X4,
				SPT_MACRO
			};
			{
				UINT16 len;
				fin.read((char*)&len, sizeof(len));

				char* pbuf;
				char tmpbuf[256];

				if (len < 256) pbuf = tmpbuf;
				else pbuf = WL_NEW_ARRAY(char, len + 1);

				if (len > 0)
				{
					fin.read(pbuf, len);
				}
				pbuf[len] = 0;

				//str = pbuf;

				if (pbuf != tmpbuf)
				{
					WL_DELETE_ARRAY(pbuf);
				}
			}
			{
				UINT16 len;
				fin.read((char*)&len, sizeof(len));

				char* pbuf;
				char tmpbuf[256];

				if (len < 256) pbuf = tmpbuf;
				else pbuf = WL_NEW_ARRAY(char, len + 1);

				if (len > 0)
				{
					fin.read(pbuf, len);
				}
				pbuf[len] = 0;

				//str = pbuf;

				if (pbuf != tmpbuf)
				{
					WL_DELETE_ARRAY(pbuf);
				}
			}
			UINT32 nparam = 0;
			fin.read((char*)&nparam, sizeof(UINT32));
			for (UINT32 i = 0; i < nparam; ++i)
			{
				ShaderParamType type;
				fin.read((char*)&type, sizeof(ShaderParamType));
				{
					UINT16 len;
					fin.read((char*)&len, sizeof(len));

					char* pbuf;
					char tmpbuf[256];

					if (len < 256) pbuf = tmpbuf;
					else pbuf = WL_NEW_ARRAY(char, len + 1);

					if (len > 0)
					{
						fin.read(pbuf, len);
					}
					pbuf[len] = 0;

					//str = pbuf;

					if (pbuf != tmpbuf)
					{
						WL_DELETE_ARRAY(pbuf);
					}
				}
				{
					UINT16 len;
					fin.read((char*)&len, sizeof(len));

					char* pbuf;
					char tmpbuf[256];

					if (len < 256)
					{
						pbuf = tmpbuf;
					}
					else
					{
						pbuf = WL_NEW_ARRAY(char, len + 1);
					}
					if (len > 0)
					{
						fin.read(pbuf, len);
					}
					pbuf[len] = 0;
				}
			}
		};

		auto fReadOtherInfo = [&](std::ifstream& fin)
		{
			Vec3F cenet;
			Vec3F exent;
			float radius;
			fin.read((char*)&cenet, sizeof(Vec3F));
			fin.read((char*)&exent, sizeof(Vec3F));
			fin.read((char*)&radius, sizeof(float));
			UINT32 bindBoneID;
			fin.read((char*)&bindBoneID, sizeof(UINT32));

			bool bSkin;
			fin.read((char*)&bSkin, sizeof(bSkin));
		};

		auto fReadSubMeshData = [&](std::ifstream& fin)
		{
			UINT32 primitiveType;
			fin.read((char*)&primitiveType, sizeof(UINT32));

			UINT32 maxVertInfl;
			fin.read((char*)&maxVertInfl, sizeof(UINT32));

			////indice Data 
			UINT32 nIndicesCount;
			fin.read((char*)&nIndicesCount, sizeof(UINT32));
			indices.resize(nIndicesCount);
			void* pData = indices.data();
			fin.read((char*)pData, sizeof(UINT16) * nIndicesCount);

			////vertices Data

			UINT32 refVertStart;
			fin.read((char*)&refVertStart, sizeof(UINT32));

			UINT32 refVertNum;
			fin.read((char*)&refVertNum, sizeof(UINT32));

			UINT32 vertfmtElement;
			fin.read((char*)&vertfmtElement, sizeof(UINT32));
			if (vertfmtElement > 0)
			{	// Vertex element type, used to identify the base types of the vertex contents
				enum VertexElementType
				{
					VET_FLOAT1 = 0,
					VET_FLOAT2 = 1,
					VET_FLOAT3 = 2,
					VET_FLOAT4 = 3,
					// alias to more specific colour type - use the current rendersystem's colour packing
					VET_COLOUR = 4,
					VET_SHORT1 = 5,
					VET_SHORT2 = 6,
					VET_SHORT3 = 7,
					VET_SHORT4 = 8,
					VET_UBYTE4 = 9,
				};

				enum VertexElementSemantic
				{
					// Position, 3 reals per vertex
					VES_POSITION = 1,
					// Blending weights
					VES_BLEND_WEIGHTS = 2,
					// Blending indices
					VES_BLEND_INDICES = 3,
					// Normal, 3 reals per vertex
					VES_NORMAL = 4,
					// Diffuse colours
					VES_COLOR = 5,
					// Texture coordinates
					VES_TEXCOORD = 7,
					// Binormal (Y axis if normal is Z)
					VES_BINORMAL = 8,
					// Tangent (X axis if normal is Z)
					VES_TANGENT = 9,

					//transformed position
					VES_POSITIONT = 10,
				};
				struct SVertexElement
				{
					UINT32 m_StreamSource : 4;
					UINT32 m_Offset : 8;
					VertexElementType m_Type : 8;
					VertexElementSemantic m_Semantic : 8;
					UINT32 m_Index : 4;

					UINT32 getTypeSize(VertexElementType etype)
					{
						switch (etype)
						{
						case VET_COLOUR:
							return sizeof(UINT32);
						case VET_FLOAT1:
							return sizeof(float);
						case VET_FLOAT2:
							return sizeof(float) * 2;
						case VET_FLOAT3:
							return sizeof(float) * 3;
						case VET_FLOAT4:
							return sizeof(float) * 4;
						case VET_SHORT1:
							return sizeof(short);
						case VET_SHORT2:
							return sizeof(short) * 2;
						case VET_SHORT3:
							return sizeof(short) * 3;
						case VET_SHORT4:
							return sizeof(short) * 4;
						case VET_UBYTE4:
							return sizeof(unsigned char) * 4;
						default:
							assert(0);
							return 0;
						}
					}
					UINT32 getSize()
					{
						return getTypeSize(m_Type);
					}
				};
				std::vector<SVertexElement> element;
				element.resize(vertfmtElement);
				if (vertfmtElement != 5)
				{
					bCreateMesh = false;
				}
				else
				{
					bCreateMesh = true;
				}
				pData = element.data();
				fin.read((char*)pData, sizeof(SVertexElement) * vertfmtElement);

				fin.read((char*)&minPos, sizeof(minPos));
				fin.read((char*)&maxPos, sizeof(maxPos));

				UINT32 verticesCount = 0;
				fin.read((char*)&verticesCount, sizeof(UINT32));
				std::vector<char> verticeData;
				verticeData.resize(verticesCount);
				pData = verticeData.data();
				fin.read((char*)pData, verticesCount);
				auto getStride = [&]()->UINT32
				{
					UINT32 stride = 0;
					for (size_t i = 0; i < element.size(); i++)
					{
						stride += element[i].getSize();
					}
					return stride;
				};
				UINT32 stride = getStride();
				size_t nVertex = verticeData.size() / stride;
				vertices.resize(nVertex);
				memcpy(vertices.data(), pData, verticesCount);
				auto buffer = fReadDataType(fin);
				if ("Material" == buffer)
				{
					fReadMaterialData(fin);
				}
				fReadPatches(fin);
			}
			fReadOtherInfo(fin);
		};


		auto fReadMeshData = [&](std::ifstream& fin)
		{
			UINT16 len;
			fin.read((char*)&len, sizeof(len));
			char* pbuf;
			char tmpbuf[256];

			if (len < 256)
			{
				pbuf = tmpbuf;
			}
			else
			{
				pbuf = WL_NEW_ARRAY(char, len + 1);
			}
			if (len > 0)
			{
				fin.read(pbuf, len);
			}
			pbuf[len] = 0;

			//str = pbuf;

			if (pbuf != tmpbuf)
			{
				WL_DELETE_ARRAY(pbuf);
			}
			UINT32 num = 0;
			fin.read((char*)&num, sizeof(UINT32));
			for (UINT32 i = 0; i < num; ++i)
			{
				auto buffer = fReadDataType(fin);
				if (buffer == "SubMeshData")
				{
					fReadSubMeshData(fin);
				}
				if (bCreateMesh)
				{
					CMesh* pMesh = dynamic_cast<CMesh*>(createResource(Mesh));
					pMesh->setName("omod");
					pMesh->fillVertexBuffer(&vertices[0], sizeof(SVertexFormatOMOD) * vertices.size(), sizeof(SVertexFormatOMOD));
					pMesh->fillIndexBuffer(&indices[0], sizeof(UINT16) * indices.size(), sizeof(UINT16));
					meshes.push_back(pMesh);
				}
			}

		};
		auto fReadModelData = [&](std::ifstream& fin)
		{
			auto buffer = fReadDataType(fin);

			if (std::string("MeshData") == buffer)
			{
				fReadMeshData(fin);
			}
		};

		std::ifstream fin;
		//FILE* pfile = fopen(strFile.c_str());
		//char input = 0;
		// Open the model file.
		fin.open(strFile.c_str(), std::ios::binary);

		// If it could not open the file then exit.
		if (fin.fail())
		{
			return meshes;
		}
		SResourceFileHeader header;
		fin.read((char*)&header, sizeof(header));

		auto buffer = fReadDataType(fin);

		UINT32 num = 0;
		fin.read((char*)&num, sizeof(UINT32));
		for (UINT32 i = 0; i < num; ++i)
		{
			if (std::string("ModelData") == buffer)
			{
				fReadModelData(fin);
			}
			else if (std::string("MeshData") == buffer)
			{
				fReadMeshData(fin);
			}
			else if (std::string("SkeletonData") == buffer)
			{
				fReadSkeletonData(fin);
			}
		}
		// Close the model file.

		fin.close();
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			calculationBoundBox(meshes[i]);
		}
		return meshes;
	}

	bool CResourceMgr::_loadMesh_m(CMesh* pMesh, const std::string& strFile)
	{
		std::ifstream fin;
		char input = 0;

		// Open the model file.
		fin.open(strFile.c_str());

		// If it could not open the file then exit.
		if (fin.fail())
		{
			return false;
		}

		// Read up to the value of vertex count.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		// Read in the vertex count.
		INT32 vertexCount = 0;
		fin >> vertexCount;

		SVertexFormat vertexFormat;
		vertexFormat.addElement(V_POSITION);
		vertexFormat.addElement(V_TEXTURE);
		vertexFormat.addElement(V_NORMAL);
		vertexFormat.addElement(V_TANGETN);
		vertexFormat.addElement(V_BINORMAL);


		// Set the number of indices to be the same as the vertex count.
		INT32 indexCount = vertexCount;
		std::vector<UINT8> vertices;
		std::vector<UINT32> indices;
		int nVertexSize = vertexFormat.getVertexSize();
		vertices.resize(vertexCount * nVertexSize);
		indices.resize(vertexCount);
		// Read up to the beginning of the data.

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		// Read in the vertex data.
		UINT8* pData = (UINT8*)vertices.data();
		for (INT32 i = 0; i < vertexCount; ++i)
		{
			float* p = (float*)pData;
			//position
			fin >> p[0] >> p[1] >> p[2];
			//uv
			fin >> p[3] >> p[4];
			//normal
			fin >> p[5] >> p[6] >> p[7];
			pData += nVertexSize;
			indices[i] = i;
		}
		// Close the model file.
		fin.close();

		pMesh->fillVertexBuffer(vertices.data(), vertices.size(), nVertexSize);
		pMesh->fillIndexBuffer(&indices[0], sizeof(UINT32) * indices.size(), sizeof(UINT32));
		return true;
	}

	bool CResourceMgr::_loadMesh_mTwo(CMesh* pMesh, const std::string& strFile)
	{
		return true;
	}

	bool CResourceMgr::_loadMesh_fbx(CMesh* pMesh, const std::string& strFile)
	{
		return true;
	}

	bool CResourceMgr::_loadMesh_txt(CMesh* pMesh, const std::string& strFile)
	{
		return true;
	}

	bool CResourceMgr::_loadMesh_sdkmesh(CResource* pResourceBase, const std::string& strFile)
	{
		return true;
	}

	void CResourceMgr::calculationBoundBox(CMesh* pMesh)
	{
		byte* pData = pMesh->getDataStream();
		int nVertexSize = pMesh->getVertexTypeSize();

		Vec3F fMin = { FLT_MAX, FLT_MAX,FLT_MAX };
		Vec3F fMax = { -FLT_MAX, -FLT_MAX,-FLT_MAX };
		if (nullptr != pData)
		{
			int nCount = pMesh->getVerticesCount();
			for (int i = 0; i < nCount; ++i)
			{
				Vec3F* pos = (Vec3F*)pData;
				if (pos->x < fMin.x)
				{
					fMin.x = pos->x;
				}
				if (pos->y < fMin.y)
				{
					fMin.y = pos->y;
				}
				if (pos->z < fMin.z)
				{
					fMin.z = pos->z;
				}

				//
				if (pos->x > fMax.x)
				{
					fMax.x = pos->x;
				}
				if (pos->y > fMax.y)
				{
					fMax.y = pos->y;
				}
				if (pos->z > fMax.z)
				{
					fMax.z = pos->z;
				}
				pData += nVertexSize;
			}
			auto& aabb = const_cast<CAABBox&>(pMesh->getBoundBox());
			aabb.setBoundBoxRange(fMin, fMax);
		}
	}

}