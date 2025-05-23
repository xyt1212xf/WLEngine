#include "VoxelTerrainEditor.h"
#include "Scene.h"
#include "MaterialInstance.h"
#include "VoxelBrush.h"

const int EdgeTable[12][2] =
{
	{0,1},
	{1,2},
	{2,3},
	{0,3},
	{4,5},
	{5,6},
	{6,7},
	{4,7},
	{0,4},
	{1,5},
	{2,6},
	{3,7}
};

namespace WL
{
	const float fPixelOffset = 1.0f / 512.0f;
	const float brushSize = 4;
	CAABBox CVoxelTerrainEditor::gVoxelGroundAABB;
	CAABBox CVoxelTerrainEditor::gVoxelTerrainAABB;
	CVoxelTerrainEditor::CVoxelTerrainEditor()
	{
		int nSizeX = 10;
		int nSizeY = 1;
		int nSizeZ = 10;
		mpVoxelMgr = WL_NEW(CVoxelMgr, Manager)();
		mpBrush = WL_NEW(CVoxelBrush, Default)();
		mpBrush->init();
		//mpBrush->setPosition(Vec3F(5, 5, 5));
		mpVoxelMgr->setBrush(mpBrush);
		mpVoxelMgr->setPositon(Vec3F(-340, 950, -2025));
		GEngine->getCurrentScenePtr()->setVoxelMgr(mpVoxelMgr);
		mpVoxelMgr->createVoxelTerrain(nSizeX, nSizeY, nSizeZ);
		mpVoxelMgr->buildTerrain(1);
		auto& v = mpVoxelMgr->getPosition();
		Vec3F fMin = { v.x, v.y, v.z };
		Vec3F fMax = { fMin.x + nSizeX * CVoxelChunk::nSize * SVoxel::nSize,
					   fMin.y + nSizeY * CVoxelChunk::nSize * SVoxel::nSize,
					   fMin.z + nSizeZ * CVoxelChunk::nSize * SVoxel::nSize };

		Vec3F p0 = { fMin.x, fMin.y, fMin.z };
		Vec3F p1 = { fMin.x, fMin.y, fMax.z };
		Vec3F p2 = { fMax.x, fMin.y, fMax.z };
		Vec3F p3 = { fMax.x, fMin.y, fMin.z };
		mGroundPlaneVertice.emplace_back(p0);
		mGroundPlaneVertice.emplace_back(p1);
		mGroundPlaneVertice.emplace_back(p2);
		mGroundPlaneVertice.emplace_back(p3);


		////////////////////////////////////////////////////////////////////////// 
#if 0
		{
			std::vector<VertexFormatVoxel> drawVertices;
			std::vector<UINT32> drawIndices;

			VertexFormatVoxel p0;
			p0.position = { -4, 0, 4 };
			p0.uv = { 0, 0 };
			p0.normal = { 0, 1, 0 };

			VertexFormatVoxel p1;
			p1.position = { 4, 0, 4 };
			p1.uv = { 1, 0 };
			p1.normal = { 0, 1, 0 };

			VertexFormatVoxel p2;
			p2.position = { 4, 0, -4 };
			p2.uv = { 1, 1 };
			p2.normal = { 0, 1, 0 };

			VertexFormatVoxel p3;
			p3.position = { -4, 0, -4 };
			p3.uv = { 0, 1 };
			p3.normal = { 0, 1, 0 };
			drawIndices.push_back(0);
			drawIndices.push_back(1);
			drawIndices.push_back(2);

			drawIndices.push_back(0);
			drawIndices.push_back(2);
			drawIndices.push_back(3);
			drawVertices.push_back(p0);
			drawVertices.push_back(p1);
			drawVertices.push_back(p2);
			drawVertices.push_back(p3);

			mpBrushPlane = GEngine->createEntity<CActorEntity>(Actor);
			auto pModelInstance = Foundation::generateModel((void*)drawVertices.data(),
				drawVertices.size(),
				sizeof(VertexFormatVoxel),
				(void*)drawIndices.data(),
				drawIndices.size(),
				sizeof(UINT32),
				"resource/Material/newVoxel.ma",
				"resource/Texture/Voxel/line.png");
			mpBrushPlane->addModel(pModelInstance);
			pModelInstance->getMaterialInstance()->setAlpha(1);
			SafeRelease(pModelInstance);
			GEngine->getCurrentScenePtr()->addEntity(mpBrushPlane);
		}
#else
		//voxel������
		//modifyBrushPlaneDir();
#endif
		GEngine->setVoxelMgr(mpVoxelMgr);
	}

	CVoxelTerrainEditor::~CVoxelTerrainEditor()
	{
		WL_DELETE(mpBrush, Default);
		WL_DECREASE(mpBrushPlane);
		WL_DECREASE(mpVoxelMgr);
	}

	bool CVoxelTerrainEditor::onEvent(SEvent& e)
	{
		switch (e.message)
		{
		case MSG_KEYDOWN:
		{
			static bool b = true;
			if ('0' == e.wParam)
			{
				mbDrawTexture = false;
			}
			else if ('1' == e.wParam)
			{
				mDrawTextureIndex = 1;
				mbDrawTexture = true;
			}
			else if ('2' == e.wParam)
			{
				mDrawTextureIndex = 2;
				mbDrawTexture = true;
			}
			else if ('3' == e.wParam)
			{
				mDrawTextureIndex = 3;
				mbDrawTexture = true;
			}
			else if ('4' == e.wParam)
			{
				mDrawTextureIndex = 4;
				mbDrawTexture = true;
			}
			else if ('5' == e.wParam)
			{
				mDrawTextureIndex = 5;
				mbDrawTexture = true;
			}
			else if ('6' == e.wParam)
			{
				mDrawTextureIndex = 6;
				mbDrawTexture = true;
			}
			else if ('7' == e.wParam)
			{
				mDrawTextureIndex = 7;
				mbDrawTexture = true;
			}
			else
			{
			//	_generateMesh();
				auto vPos = mpBrush->getPosition();
				auto pCamera = GEngine->getMainCameraEntity();
				if (nullptr != pCamera)
				{
					auto mt = *pCamera->getViewMatrixPtr()* (*pCamera->getViewMatrixPtr());

					{
						auto i = Foundation::changeToScreenCoordinate(vPos);
						int x = 0;

					}
					auto xy = Math::matrixMulVec3(mt, vPos);
					float sx = pCamera->getViewSize().x * 0.5f;
					float sy = pCamera->getViewSize().y * 0.5f;
					{
						float x = (xy.x / xy.z) + 0.5f;
						float y = (xy.y / -xy.z) + 0.5f;
						float z = 0;
					}
					float x = (xy.x / xy.z) * 0.5f + 0.5f ;
					float y = (xy.y / xy.z) * 0.5f + 0.5f;
					x = x * pCamera->getViewSize().x;
					y = (1 - y) * pCamera->getViewSize().y;
					float z = xy.y / xy.z;
				}
			}
			//e.bProcess = true;
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
			break;
		}
		case MSG_MOUSEMOVE:
		{
#if 0 //��ʱ�Ȳ�ˢ������
			if (mMouseFlag & (1 << 0))
			{
				generateVoxelChunk(e);
			}
			else
			{
				modifyBrushPlaneDir();
				mouseBrushPlane(e);
			}
#endif
			break;
		}
		case MSG_MOUSEWHEEL:
		{
			break;
		}
		default:
			return false;
		}
		return true;
	}

	void CVoxelTerrainEditor::generateVoxelChunk(SEvent& e)
	{
		CRay ray(GEngine->getMainCameraEntity());
		ray.setScreenMouseSpace(e.mouseX, e.mouseY);
		float t = FLT_MAX;
		for (auto pChunk : mpVoxelMgr->mpChunks)
		{
			if (ray.intersectAABB(pChunk->getAABB()))
			{
				const auto& v = mpBrushPlane->getPosition();

				Vec3F p0;
				p0 = { -2, 0, 2 };
				p0 += v;
				Vec3F p1;
				p1 = { 2, 0, 2 };
				p1 += v;

				Vec3F p2;
				p2 = { 2, 0, -2 };
				p2 += v;

				Vec3F p3;
				p3 = { -2, 0, -2 };
				p3 += v;

				if (ray.intersectTriangle(&p0, &p1, &p2) ||
					ray.intersectTriangle(&p0, &p2, &p3))
				{
					if (ray.mResult.fDist < t)
					{
						t = ray.mResult.fDist;
					}
				}
			}
		}

		bool bPick = true;
		if (FLT_MAX == t)
		{
			bPick = ray.intersectAABB(CVoxelTerrainEditor::gVoxelGroundAABB);
		}
		if (bPick)
		{
			auto vPos = ray.mDirection * ray.mResult.fDist + ray.mOriginPosition;
			mpBrush->setPosition(vPos);
			mpBrushPlane->setPosition(vPos);
			if (mbDrawTexture)
			{
				mouseBrushPlane(e);
				_generateTexture();
			}
			else
			{
				_generateMesh();
			}
		}
	}

	void CVoxelTerrainEditor::_generateMesh()
	{
		auto processNormalLambda = [&](const Vec3F& v0, const Vec3F& v1, const Vec3F& v2)->Vec3F
		{
			Vec3F vNormal;
			Vec3F vDir0 = v1 - v0;
			Vec3F vDir1 = v2 - v0;

			Math::vec3Cross(&vNormal, &vDir0, &vDir1);
			vNormal.normalize();
			return vNormal;
		};


		const auto& AABB = mpBrush->getBoundBox();
		auto& vCenter = AABB.getCenter();

		int tempIndices[CVoxelChunk::nSize +1][CVoxelChunk::nSize +1][CVoxelChunk::nSize +1];

		auto generateFaceLambda = [&](SGeometryInfo& geometry, const Vec3I& p0, const Vec3I& p1, const Vec3I& p2, const Vec3I& p3)
		{
			if (tempIndices[p0.x][p0.y][p0.z] > 0 &&
				tempIndices[p1.x][p1.y][p1.z] > 0 &&
				tempIndices[p2.x][p2.y][p2.z] > 0 &&
				tempIndices[p3.x][p3.y][p3.z] > 0)
			{
				SVertexFormatVoxel& v0 = geometry.vertices[p0.x][p0.y][p0.z];
				SVertexFormatVoxel& v1 = geometry.vertices[p1.x][p1.y][p1.z];
				SVertexFormatVoxel& v2 = geometry.vertices[p2.x][p2.y][p2.z];
				SVertexFormatVoxel& v3 = geometry.vertices[p3.x][p3.y][p3.z];

				v0.uv.x = fPixelOffset;
				v0.uv.y = fPixelOffset;

				v1.uv.x = 0.25f - fPixelOffset;
				v1.uv.y = fPixelOffset;

				v2.uv.x = fPixelOffset;
				v2.uv.y = 0.25f - fPixelOffset;

				v3.uv.x = 0.25f - fPixelOffset;
				v3.uv.y = 0.25f - fPixelOffset;

				SFace face;
				face.pos[0] = p0;
				face.vPos[0] = v0;
				{
					auto&& vNormal = processNormalLambda(v0.position, v2.position, v3.position);
					auto vTemp = (v0.position + v2.position + v3.position) / 3.0f - vCenter;
					vTemp.normalize();
					if (Math::vec3Dot(&vTemp, &vNormal) < 0)
					{
						face.pos[1] = p3;
						face.vPos[1] = v3;
						
						face.pos[2] = p2;
						face.vPos[2] = v2;
					}
					else
					{
						face.pos[1] = p2;
						face.vPos[1] = v2;

						face.pos[2] = p3;
						face.vPos[2] = v3;
					}
					auto iter = std::find(geometry.faces.begin(), geometry.faces.end(), face);
					if (iter == geometry.faces.end())
					{
						geometry.faces.push_back(face);
					}
				}
				{
					auto&& vNormal = processNormalLambda(v0.position, v3.position, v1.position);
					auto vTemp = (v0.position + v3.position + v1.position) / 3.0f - vCenter;
					vTemp.normalize();
					if (Math::vec3Dot(&vTemp, &vNormal) < 0)
					{
						face.pos[1] = p1;
						face.vPos[1] = v1;
						
						face.pos[2] = p3;
						face.vPos[2] = v3;
					}
					else
					{
						face.pos[1] = p3;
						face.vPos[1] = v3;
						
						face.pos[2] = p1;
						face.vPos[2] = v1;
					}
					auto iter = std::find(geometry.faces.begin(), geometry.faces.end(), face);
					if (iter == geometry.faces.end())
					{
						geometry.faces.push_back(face);
					}
				}
			}
		};

		for (auto pChunk : mpVoxelMgr->mpChunks)
		{
			if (AABB.checkBoundBox(pChunk->getAABB()))
			{
				INT64 uuid = pChunk->mUUID;
				ZeroMemory(mChunkGeometryInfo[uuid].vertexNormalCount, sizeof(mChunkGeometryInfo[uuid].vertexNormalCount));
				ZeroMemory(mChunkGeometryInfo[uuid].vertexNormal, sizeof(mChunkGeometryInfo[uuid].vertexNormal));

				ZeroMemory(tempIndices, sizeof(tempIndices));

				auto& voxels = pChunk->getVoxels();
				for (auto voxel : voxels)
				{
					int nX = voxel->nX;
					int nY = voxel->nY;
					int nZ = voxel->nZ;
					if (voxel->aabb.checkBoundBox(AABB))
					{
						auto&& v = mpBrush->getIsoSurfaceVec3(pChunk, voxel);
						if (v.first)
						{
							SVertexFormatVoxel vec;
							vec.position = v.second;
							vec.normal = { 0, 0, 0 };
							vec.uv = { 0, 0 };
							mChunkGeometryInfo[uuid].vertices[nX][nY][nZ] = vec;
							tempIndices[nX][nY][nZ] = 1;
						}
					}

				}
				Vec3I p0, p1, p2, p3;
				for (int x = 0; x < CVoxelChunk::nSize; ++x)
				{
					for (int y = 0; y < CVoxelChunk::nSize; ++y)
					{
						for (int z = 0; z < CVoxelChunk::nSize; ++z)
						{
							p0.x = x;
							p0.y = y;
							p0.z = z;

							//xy plane
							p1.x = x + 1;
							p1.y = y;
							p1.z = z;

							p2.x = x;
							p2.y = y + 1;
							p2.z = z;

							p3.x = x + 1;
							p3.y = y + 1;
							p3.z = z;
							generateFaceLambda(mChunkGeometryInfo[uuid], p0, p1, p2, p3);

							// xz plane					
							p1.x = x + 1;
							p1.y = y;
							p1.z = z;

							p2.x = x;
							p2.y = y;
							p2.z = z + 1;

							p3.x = x + 1;
							p3.y = y;
							p3.z = z + 1;
							generateFaceLambda(mChunkGeometryInfo[uuid], p0, p1, p2, p3);

							// yz plane						
							p1.x = x;
							p1.y = y;
							p1.z = z + 1;

							p2.x = x;
							p2.y = y + 1;
							p2.z = z;

							p3.x = x;
							p3.y = y + 1;
							p3.z = z + 1;
							generateFaceLambda(mChunkGeometryInfo[uuid], p0, p1, p2, p3);
						}
					}
				}

				pChunk->mVertices.clear();
				pChunk->mDrawVertices.clear();
				pChunk->mDrawIndices.clear();

				//calculate faces normal
				for (auto& item : mChunkGeometryInfo[uuid].faces)
				{
					auto&& vNormal = processNormalLambda(item.vPos[0].position, item.vPos[1].position, item.vPos[2].position);
					vNormal.normalize();
					for( int i = 0; i < 3; ++i )
					{
						int x = item.pos[i].x;
						int y = item.pos[i].y;
						int z = item.pos[i].z;
						mChunkGeometryInfo[uuid].vertexNormal[x][y][z] += vNormal;
						mChunkGeometryInfo[uuid].vertexNormalCount[x][y][z] += 1;
					}
				}
				//calculate vertices normal
				for (auto& item : mChunkGeometryInfo[uuid].faces)
				{
					for (int i = 0; i < 3; ++i)
					{
						int x = item.pos[i].x;
						int y = item.pos[i].y;
						int z = item.pos[i].z;
						mChunkGeometryInfo[uuid].vertices[x][y][z].normal = mChunkGeometryInfo[uuid].vertexNormal[x][y][z] / mChunkGeometryInfo[uuid].vertexNormalCount[x][y][z];
					}
				}

				for (auto& item : mChunkGeometryInfo[uuid].faces)
				{
					int nSize = (int)pChunk->mDrawIndices.size();
					for (int i = 0; i < 3; ++i)
					{
						int x = item.pos[i].x;
						int y = item.pos[i].y;
						int z = item.pos[i].z;
						auto v = mChunkGeometryInfo[uuid].vertices[x][y][z];
						v.uv = item.vPos[i].uv;
						pChunk->mDrawVertices.emplace_back(v);
						pChunk->mVertices.emplace_back(&mChunkGeometryInfo[uuid].vertices[x][y][z]);
						pChunk->mDrawIndices.push_back(nSize + i);
					}
				}
#ifdef _DEBUG
#if 0
				drawNormal(pChunk, mChunkGeometryInfo[uuid]);
#endif
#endif
				pChunk->generateGeometry();
			}
		}
	}


	void CVoxelTerrainEditor::_generateTexture()
	{
		const auto& AABB = mpBrush->getBoundBox();
		for (auto pChunk : mpVoxelMgr->mpChunks)
		{
			if (AABB.checkBoundBox(pChunk->getAABB()))
			{
				bool bReBuildGeometry = false;
				for (size_t i = 0; i < pChunk->mDrawVertices.size(); ++i)
				{
					auto& v = pChunk->mDrawVertices[i];
					if (AABB.checkPoint(v.position))
					{
						v.alphaIndex.x = mDrawTextureIndex % 4 * 0.25f;
						v.alphaIndex.y = mDrawTextureIndex / 4 * 0.25f;
						v.alphaIndex.z = 1;
						bReBuildGeometry = true;
						pChunk->mVertices[i]->alphaIndex = v.alphaIndex;
					}
				}
				if (bReBuildGeometry)
				{
					pChunk->generateGeometry();
				}
			}
		}
	}


	void CVoxelTerrainEditor::drawNormal(CVoxelChunk* pChunk, SGeometryInfo& geometry)
	{
		if (pChunk->mDrawVertices.size() > 0)
		{
			if (nullptr != pChunk->mpNormalEntity)
			{
				GEngine->getCurrentScenePtr()->removeEntity(pChunk->mpNormalEntity);
			}
			std::vector<SVertexFormatVoxel> vertices;
			std::vector<UINT16> indices;
			int i = 0;
			for (auto item : pChunk->mDrawVertices)
			{
				vertices.push_back(item);
				SVertexFormatVoxel v = item;
				v.position += v.normal;
				vertices.push_back(v);
				indices.push_back(i++);
				indices.push_back(i++);
			}
			int nSize = static_cast<int>(vertices.size());

			auto pModelInstance = Foundation::generateModel(vertices.data(), nSize, sizeof(SVertexFormatVoxel), indices.data(), static_cast<int>(indices.size()), sizeof(UINT16),
				"resource/Material/newVoxel.ma",
				"resource/Texture/Voxel/normal.png");
			pModelInstance->getMaterialInstance()->setTopology(PRIMITIVE_TOPOLOGY_LINELIST);
			auto pMaterialInstance = pModelInstance->getMaterialInstance();
			if (pMaterialInstance != nullptr)
			{
				pMaterialInstance->bindTexture(TextureUseType::Normal, "Resource/Texture/Voxel/wood.png");
			}
			auto pNormalEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);
			pNormalEntity->addModel(pModelInstance);
			pChunk->mpNormalEntity = pNormalEntity;
			GEngine->getCurrentScenePtr()->addEntity(pNormalEntity);
			WL_DECREASE(pModelInstance);
			WL_DECREASE(pNormalEntity);
		}
	}

	void CVoxelTerrainEditor::mouseBrushPlane(SEvent& e)
	{
		CRay ray(GEngine->getMainCameraEntity());
		ray.setScreenMouseSpace(e.mouseX, e.mouseY);

		float t = FLT_MAX;
		for (auto pChunk : mpVoxelMgr->mpChunks)
		{
			if (ray.intersectAABB(pChunk->getAABB()))
			{
				int nSize = (int)pChunk->mDrawIndices.size();
				UINT32* pData = pChunk->mDrawIndices.data();
				for (int i = 0; i < nSize; i += 3)
				{
					int nIdx0 = pData[i + 0];
					int nIdx1 = pData[i + 1];
					int nIdx2 = pData[i + 2];
					if (ray.intersectTriangle(&pChunk->mDrawVertices[nIdx0].position,
						&pChunk->mDrawVertices[nIdx1].position,
						&pChunk->mDrawVertices[nIdx2].position))
					{
						if (ray.mResult.fDist < t)
						{
							t = ray.mResult.fDist;
						}
					}
				}
			}
		}

		bool bPick = true;
		if (FLT_MAX == t)
		{
			bPick = ray.intersectAABB(CVoxelTerrainEditor::gVoxelGroundAABB);
		}
		if (bPick)
		{
			auto vPos = ray.mDirection * ray.mResult.fDist + ray.mOriginPosition;
			mpBrush->setPosition(vPos);
			mpBrushPlane->setPosition(vPos);
		}
	}

	void CVoxelTerrainEditor::modifyBrushPlaneDir()
	{
		auto pCamera = GEngine->getMainCameraEntity();
		if (nullptr != pCamera)
		{
			const auto& eye = pCamera->getEye();
			const auto& lookAt = pCamera->getLookAt();
			auto&& v3f = lookAt - eye;
			float fScale = v3f.z / v3f.y;
			SVertexFormatVoxel p0;
			SVertexFormatVoxel p1;
			SVertexFormatVoxel p2;
			SVertexFormatVoxel p3;

			p0.uv = { 0, 0 };
			p0.normal = { 0, 1, 0 };

			p1.uv = { 1, 0 };
			p1.normal = { 0, 1, 0 };

			p2.uv = { 1, 1 };
			p2.normal = { 0, 1, 0 };

			p3.uv = { 0, 1 };
			p3.normal = { 0, 1, 0 };

			if ( fabs(fScale) > 0.5f)
			{
				v3f.y = 0;
				v3f.normalize();
				
				Math::vec3RotationY(&v3f, Math::angleToRadian(90.0f));
				v3f *= brushSize;

				p0.position = { -v3f.x, brushSize, 0 };

				p1.position = { v3f.x, brushSize, 0 };

				p2.position = { v3f.x, -brushSize, 0 };

				p3.position = { -v3f.x, -brushSize, 0 };
			}
			else
			{
				p0.position = { -brushSize, 0, brushSize };

				p1.position = { brushSize, 0, brushSize };

				p2.position = { brushSize, 0, -brushSize };

				p3.position = { -brushSize, 0, -brushSize };
			}

			std::vector<SVertexFormatVoxel> drawVertices;
			std::vector<UINT32> drawIndices;

			drawIndices.push_back(0);
			drawIndices.push_back(1);
			drawIndices.push_back(2);

			drawIndices.push_back(0);
			drawIndices.push_back(2);
			drawIndices.push_back(3);
			drawVertices.push_back(p0);
			drawVertices.push_back(p1);
			drawVertices.push_back(p2);
			drawVertices.push_back(p3);

			auto pEntity = mpBrushPlane;
			mpBrushPlane = GEngine->createEntity<CActorEntity>(Actor);
			auto pModelInstance = Foundation::generateModel((void*)drawVertices.data(),
				static_cast<int>(drawVertices.size()),
				sizeof(SVertexFormatVoxel),
				(void*)drawIndices.data(),
				static_cast<int>(drawIndices.size()),
				sizeof(UINT32),
				"resource/Material/newVoxel.ma",
				"resource/Texture/Voxel/line.png");
			mpBrushPlane->addModel(pModelInstance);
			pModelInstance->getMaterialInstance()->setAlpha(1);
			WL_DECREASE(pModelInstance);
			GEngine->getCurrentScenePtr()->addEntity(mpBrushPlane);
			GEngine->getCurrentScenePtr()->removeEntity(pEntity);
		}
	}

}