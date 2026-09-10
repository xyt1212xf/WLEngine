#include "OceanSimulator.h"
#include "WLEngine.h"

namespace WL
{

	float estimateGridCoverage(const SQuadNode& quad_node, float screen_area)
	{
		// Estimate projected area

		// Test 16 points on the quad and find out the biggest one.
		const static float sample_pos[4][2] =
		{
			{0, 0},
			{0, 1},
			{1, 0},
			{1, 1},
			//{0.5f, 0.333f},
			//{0.25f, 0.667f},
			//{0.75f, 0.111f},
			//{0.125f, 0.444f},
			//{0.625f, 0.778f},
			//{0.375f, 0.222f},
			//{0.875f, 0.556f},
			//{0.0625f, 0.889f},
			//{0.5625f, 0.037f},
			//{0.3125f, 0.37f},
			//{0.8125f, 0.704f},
			//{0.1875f, 0.148f},
		};

		SMatrix44 matProj = *GEngine->getProjectMatrix();
		Vec3F eye_point = GEngine->getMainCameraEntity()->getEye();
		//eye_point = D3DXVECTOR3(eye_point.x, eye_point.z, eye_point.y);
		float grid_len_world = quad_node.length / CD3D11OceanSimulator::sMeshDim;

		float max_area_proj = 0;
		for (int i = 0; i < 4; i++)
		{
			Vec3F test_point(quad_node.bottom_left.x + quad_node.length * sample_pos[i][0], quad_node.bottom_left.y + quad_node.length * sample_pos[i][1], 0);
			Vec3F eye_vec = test_point - eye_point;
			float dist = eye_vec.length();

			float area_world = grid_len_world * grid_len_world;// * abs(eye_point.z) / sqrt(nearest_sqr_dist);
			float area_proj = area_world * matProj(0, 0) * matProj(1, 1) / (dist * dist);

			if (max_area_proj < area_proj)
			{
				max_area_proj = area_proj;
			}
		}

		float pixel_coverage = max_area_proj * screen_area * 0.25f;

		return pixel_coverage;
	}

	COceanSimulator::COceanSimulator()
	{

	}

	COceanSimulator::~COceanSimulator()
	{
	}

	bool COceanSimulator::initialise()
	{
		initParam();
		if (CD3D11OceanSimulator::initialise())
		{
			initNodes();
			return true;
		}
		return false;
	}

	void COceanSimulator::setOceanModelInstance(COceanModelInstance* pModelInstance)
	{
		mpModelInstance = pModelInstance;
	}

	void COceanSimulator::initNodes()
	{
		int nFurthestCover = 8;
		auto windDir = mParam.wind_dir;

		mMeshInstancesPool.insert(mMeshInstancesPool.end(), mMeshInstances.begin(), mMeshInstances.end());
		mMeshInstances.clear();

		mRenderNodes.clear();
		float ocean_extent = sPatchLength * (1 << nFurthestCover);
		SQuadNode root_node = { Vec2F(-ocean_extent * 0.5f, -ocean_extent * 0.5f), ocean_extent, 0, {-1,-1,-1,-1} };
		buildNodeList(root_node);

		// We assume the center of the ocean surface at (0, 0, 0).
		static std::list<SMeshInstanceInfo*> tempMeshInstances;
		
		SMatrix44 matView = SMatrix44(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * GEngine->getMainCameraEntity()->getViewMatrix();
		SMatrix44 matProj = GEngine->getMainCameraEntity()->getProjectMatrix();
	
		for (size_t i = 0; i < mRenderNodes.size(); i++)
		{
			SQuadNode& node = mRenderNodes[i];
			if (!isLeaf(node))
			{
				continue;
			}
			
			// Check adjacent patches and select mesh pattern
			SQuadRenderParam& render_param = selectMeshPattern(node);

			// Find the right LOD to render
			int level_size = sMeshDim;
			for (int lod = 0; lod < node.lod; lod++)
			{
				level_size >>= 1;
			}
			//auto pInstance = getMeshInstance();

			////pInstance->pMaterialInstance = mpModelInstance->getMaterialInstance();
			//pInstance->pMeshInstance->setViewMT(&matView);
			//pInstance->pMeshInstance->setProjectMT(&matProj);
			//pInstance->pMeshInstance->setPosition(Vec3F(node.bottom_left.x, node.bottom_left.y, 0.0f));
					
			int nIndex = 0;
			// Perform draw call
			if (render_param.num_inner_faces > 0)
			{
				auto pInstance = getMeshInstance();

				//pInstance->pMaterialInstance = mpModelInstance->getMaterialInstance();
				pInstance->pMeshInstance->setViewMT(&matView);
				pInstance->pMeshInstance->setProjectMT(&matProj);
				pInstance->pMeshInstance->setPosition(Vec3F(node.bottom_left.x, node.bottom_left.y, 0.0f));

				// Inner mesh of the patch
				pInstance->pMeshInstance->setIndexCount(render_param.num_inner_faces + 2);
				pInstance->pMeshInstance->setStartIndexLocation(render_param.inner_start_index);
				pInstance->pMaterialInstance->setTopology(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				tempMeshInstances.push_back(pInstance);
				node.pMeshInstanceInfo[nIndex++] = pInstance;
			}

			if (render_param.num_boundary_faces > 0)
			{
				auto pInstance = getMeshInstance();

				//pInstance->pMaterialInstance = mpModelInstance->getMaterialInstance();
				pInstance->pMeshInstance->setViewMT(&matView);
				pInstance->pMeshInstance->setProjectMT(&matProj);
				pInstance->pMeshInstance->setPosition(Vec3F(node.bottom_left.x, node.bottom_left.y, 0.0f));

				// Boundary mesh of the patch
				pInstance->pMeshInstance->setIndexCount(render_param.num_boundary_faces * 3);
				pInstance->pMeshInstance->setStartIndexLocation(render_param.boundary_start_index);
				pInstance->pMaterialInstance->setTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				tempMeshInstances.push_back(pInstance);
				node.pMeshInstanceInfo[nIndex] = pInstance;
			}
		}

		if (nullptr != mpModelInstance)
		{
			mpModelInstance->setVisual(false);
			mpModelInstance->clearMeshs();
		}
		for (auto item : tempMeshInstances)
		{
			mpModelInstance->addMeshInstance(*item);
		}
		mpModelInstance->setVisual(true);
	}


	int COceanSimulator::buildNodeList(SQuadNode& quad_node)
	{
		// Check against view frustum
		/* xyt need
		//if (!checkNodeVisibility(quad_node, camera))
		//	return -1;
		*/
		// Estimate the min grid coverage
		UINT num_vps = 1;
		const Vec2I& vp = GEngine->getMainCameraEntity()->getViewSize();
		float min_coverage = estimateGridCoverage(quad_node, (float)vp.x * vp.y);

		// Recursively attatch sub-nodes.
		bool visible = true;
		if (min_coverage > sUpperGridCoverage && quad_node.length > sPatchLength)
		{
			// Recursive rendering for sub-quads.
			SQuadNode sub_node_0 = { quad_node.bottom_left, quad_node.length / 2, 0, {-1, -1, -1, -1} };
			quad_node.sub_node[0] = buildNodeList(sub_node_0);

			SQuadNode sub_node_1 = { quad_node.bottom_left + Vec2F(quad_node.length / 2, 0), quad_node.length / 2, 0, {-1, -1, -1, -1} };
			quad_node.sub_node[1] = buildNodeList(sub_node_1);

			SQuadNode sub_node_2 = { quad_node.bottom_left + Vec2F(quad_node.length / 2, quad_node.length / 2), quad_node.length / 2, 0, {-1, -1, -1, -1} };
			quad_node.sub_node[2] = buildNodeList(sub_node_2);

			SQuadNode sub_node_3 = { quad_node.bottom_left + Vec2F(0, quad_node.length / 2), quad_node.length / 2, 0, {-1, -1, -1, -1} };
			quad_node.sub_node[3] = buildNodeList(sub_node_3);

			visible = !isLeaf(quad_node);
		}

		if (visible)
		{
			// Estimate mesh LOD
			int lod = 0;
			for (lod = 0; lod < mLods - 1; lod++)
			{
				if (min_coverage > sUpperGridCoverage)
					break;
				min_coverage *= 4;
			}

			// We don't use 1x1 and 2x2 patch. So the highest level is g_Lods - 2.
			quad_node.lod = min(lod, mLods - 2);
		}
		else
		{
			return -1;
		}
		// Insert into the list
		int position = (int)mRenderNodes.size();
		mRenderNodes.emplace_back(quad_node);
		return position;
	}

	SQuadRenderParam& COceanSimulator::selectMeshPattern(const SQuadNode& quad_node)
	{
		Vec2F point_left = quad_node.bottom_left + Vec2F(-sPatchLength * 0.5f, quad_node.length * 0.5f);
		int left_adj_index = searchLeaf(mRenderNodes, point_left);

		Vec2F point_right = quad_node.bottom_left + Vec2F(quad_node.length + sPatchLength * 0.5f, quad_node.length * 0.5f);
		int right_adj_index = searchLeaf(mRenderNodes, point_right);

		Vec2F point_bottom = quad_node.bottom_left + Vec2F(quad_node.length * 0.5f, -sPatchLength * 0.5f);
		int bottom_adj_index = searchLeaf(mRenderNodes, point_bottom);

		Vec2F point_top = quad_node.bottom_left + Vec2F(quad_node.length * 0.5f, quad_node.length + sPatchLength * 0.5f);
		int top_adj_index = searchLeaf(mRenderNodes, point_top);

		int left_type = 0;
		if (left_adj_index != -1 && mRenderNodes[left_adj_index].length > quad_node.length * 0.999f)
		{
			SQuadNode adj_node = mRenderNodes[left_adj_index];
			float scale = adj_node.length / quad_node.length * (sMeshDim >> quad_node.lod) / (sMeshDim >> adj_node.lod);
			if (scale > 3.999f)
			{
				left_type = 2;
			}
			else if (scale > 1.999f)
			{
				left_type = 1;
			}
		}

		int right_type = 0;
		if (right_adj_index != -1 && mRenderNodes[right_adj_index].length > quad_node.length * 0.999f)
		{
			SQuadNode adj_node = mRenderNodes[right_adj_index];
			float scale = adj_node.length / quad_node.length * (sMeshDim >> quad_node.lod) / (sMeshDim >> adj_node.lod);
			if (scale > 3.999f)
			{
				right_type = 2;
			}
			else if (scale > 1.999f)
			{
				right_type = 1;
			}
		}

		int bottom_type = 0;
		if (bottom_adj_index != -1 && mRenderNodes[bottom_adj_index].length > quad_node.length * 0.999f)
		{
			SQuadNode adj_node = mRenderNodes[bottom_adj_index];
			float scale = adj_node.length / quad_node.length * (sMeshDim >> quad_node.lod) / (sMeshDim >> adj_node.lod);
			if (scale > 3.999f)
			{
				bottom_type = 2;
			}
			else if (scale > 1.999f)
			{
				bottom_type = 1;
			}
		}

		int top_type = 0;
		if (top_adj_index != -1 && mRenderNodes[top_adj_index].length > quad_node.length * 0.999f)
		{
			SQuadNode adj_node = mRenderNodes[top_adj_index];
			float scale = adj_node.length / quad_node.length * (sMeshDim >> quad_node.lod) / (sMeshDim >> adj_node.lod);
			if (scale > 3.999f)
			{
				top_type = 2;
			}
			else if (scale > 1.999f)
			{
				top_type = 1;
			}
		}

		// Check lookup table, [L][R][B][T]
		return mMeshPatterns[quad_node.lod][left_type][right_type][bottom_type][top_type];
	}

}