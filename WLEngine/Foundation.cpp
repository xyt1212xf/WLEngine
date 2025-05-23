#include "Foundation.h"
#include "WLEngine.h"
#include "ActorEntity.h"
#include "ResourceMgr.h"
#include "Scene.h"
namespace WL
{
	namespace Foundation
	{
		std::string readJsonFile(const std::string& szName)
		{
			std::string szContent = "";
			std::ifstream file(szName.c_str());
			if (!file)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << szName.c_str() << " opening failed\n";
				SetConsoleTextAttribute(hConsole, 7);
				return "";
			}
			std::string content(std::istreambuf_iterator<char>(file), {});
			file.close();
			szContent = std::move(content);
			return szContent;
		}

		CModelInstance* generateFanGeometry(const Vec3F& vOrigin,
										  const Vec3F& fBeginPos,
										  const Vec3F& fEndPos,
										  const std::string& szMaterial,
										  const std::string& szTexture)
		{
			std::vector<SVertexFormatVTN> drawVertices;
			std::vector<UINT16> drawIndices;

			SVertexFormatVTN v0, v1, v2;
			v0.position = vOrigin;
			v1.position = fBeginPos;
			v2.position = fEndPos;


			Vec3F vBeginDir = fBeginPos - vOrigin;
			Vec3F vEndDir = fEndPos - vOrigin;

			float fLength = vBeginDir.length();

			vBeginDir.normalize();
			vEndDir.normalize();
			float fAngle = Math::vec3DotAngle(&vBeginDir, &vEndDir);


			drawVertices.push_back(v0);
			for (float i = 0; i < fAngle; i += 1.0f)
			{
				auto v = Math::lerp(fBeginPos, fEndPos, i / fAngle);
				v -= vOrigin;
				v.normalize();
				v1.position = vOrigin + v * fLength;
				drawVertices.push_back(v1);
			}
			v1.position = vOrigin + vEndDir * fLength;
			drawVertices.push_back(v1);

			for (INT16 i = 0; i < (INT16)drawVertices.size() - 1; ++i)
			{
				drawIndices.push_back(0);
				drawIndices.push_back(i);
				drawIndices.push_back(i + 1);
			}
			auto pModelInstance = generateModel(drawVertices.data(),
								   drawVertices.size(),
								   sizeof(SVertexFormatVTN),
								   drawIndices.data(),
								   drawIndices.size(),
								   sizeof(UINT16), 
								   szMaterial,
								   szTexture);
			pModelInstance->setName("FanGeometry");
			return pModelInstance;
		}


		CModelInstance* generateModel(void* pVertices,
									  int nVertexCount,
									  int nVertexFormatSize,
									  void* pIndices,
									  int nIndexCount,
									  int nIndexFormatSize,
									  const std::string& szMaterial,
									  const std::string& szTexture,
									  const std::string& szTextureType/* = "baseTexture"*/)
		{
			auto pModel = dynamic_cast<CModel*>(GEngine->createResource(Model));
			pModel->addMesh(szMaterial,
							pVertices,
							nVertexCount * nVertexFormatSize,
							nVertexFormatSize,
							pIndices,
							nIndexCount * nIndexFormatSize,
							nIndexFormatSize);
			if (szTexture.length() > 0)
			{
				pModel->addTexture(szTextureType, szTexture);
			}
			CModelInstance* pInstance = WL_NEW(CModelInstance, Instance)(pModel);
			return pInstance;
		}

		CModelInstance* generateBaseGeometryMesh(GeomeTryType type, const std::string& szMaterial, const std::string& szTexture, const std::string& szTextureType /*= "baseTexture"*/)
		{
			auto pModel = dynamic_cast<CModel*>(GEngine->createResource(Model));
			switch (type)
			{
			case GeomeTryType::CUBOID:
				break;
			case GeomeTryType::WEDGE:
				break;
			case GeomeTryType::PYRAMID:
				break;
			case GeomeTryType::CYLINDER:
				break;
			case GeomeTryType::CONE:
				break;
			case GeomeTryType::SPHERE:
				break;
			case GeomeTryType::COMPOSITE:
				break;
			default:
				WL_DECREASE(pModel);
				return nullptr;
			}
			CModelInstance* pInstance = WL_NEW(CModelInstance, Instance)(pModel);
			return pInstance;
		}

		const std::string getExterName(const std::string& strName)
		{
			if (std::string::npos == strName.rfind("."))
			{
				return "";
			}
			else
			{
				return strName.substr(strName.rfind(".") + 1);
			}
		}

		Vec2I changeToScreenCoordinate(const Vec3F& vPos)
		{
			Vec2I p(-1, -1);
			auto pCamera = GEngine->getMainCameraEntity();
			if (nullptr != pCamera)
			{
				return  pCamera->changeToScreenCoordinate(vPos);
			}
			else
			{
				return p;
			}
		}

		std::string getFileExtension(const std::string& name)
		{
			std::string strExtension = "";
			std::vector<std::string>&& result = splitString<std::string>(name, ".");
			if (result.size() > 1)
			{
				strExtension = result[1];
				std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);
			}
			return strExtension;
		}

		std::string getFile(const std::string& name)
		{
			std::string strExtension = "";
			std::vector<std::string>&& result = splitString<std::string>(name, ".");
			if (result.size() > 1)
			{
				strExtension = result[0];
				std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);
			}
			return strExtension;
		}

		std::string getFilePath(const std::string& strName)
		{
			auto size = strName.rfind('\\');
			if (std::string::npos == size)
			{
				size = strName.rfind('/');
				if (std::string::npos != size)
				{
					return strName.substr(0, size + 1);
				}
			}
			else
			{
				return strName.substr(0, size + 1);
			}
			return "";
		}

		std::tuple< std::vector<std::string>, std::vector<std::string> >
			getFileInfoList(std::string&& stringDir)
		{
			_finddata_t fileData;
			std::vector<std::string> strFileDir;
			std::vector<std::string> strFileName;
			long handle = 0;
			stringDir.append("/*.*");
			handle = _findfirst(stringDir.c_str(), &fileData);
			if (handle != -1)
			{
				std::string filterString[2] = { {".."}, {"."} };
				auto filter = [&](const std::string&& str)
				{
					for (auto& item : filterString)
					{
						if (item == str)
						{
							return;
						}
					}
					strFileDir.push_back(str);
				};
				do
				{
					if (fileData.attrib & _A_SUBDIR)
					{
						filter(fileData.name);
					}
					else
					{
						strFileName.push_back(fileData.name);
					}
				} while (_findnext(handle, &fileData) == 0);
				_findclose(handle);
			}
			std::tuple< std::vector<std::string>, std::vector<std::string> > result(strFileDir, strFileName);
			return result;
		}
	}
}

