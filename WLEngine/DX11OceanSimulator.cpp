#include <d3d10_1.h>
#include <D3DX10math.h>
#include "DX11OceanSimulator.h"
#include "WLEngine.h"
#include "CameraEntity.h"

#pragma comment(lib, "D3DX10.lib")

#define MESH_INDEX_2D(x, y)	(((y) + vert_rect.bottom) * (nMeshDim + 1) + (x) + vert_rect.left)
#define FRESNEL_TEX_SIZE			256
#define PERLIN_TEX_SIZE				64
// Blending term for sky cubemap
const float g_SkyBlending = 16.0f;
float g_PerlinSize = 1.0f;
float g_PerlinSpeed = 0.06f;

namespace WL
{
	// Generate boundary mesh for a patch. Return the number of generated indices
	int generateInnerMesh(RECT vert_rect, DWORD* output, int nMeshDim)
	{
		int counter = 0;
		int width = vert_rect.right - vert_rect.left;
		int height = vert_rect.top - vert_rect.bottom;

		bool reverse = false;
		for (int i = 0; i < height; i++)
		{
			if (reverse == false)
			{
				output[counter++] = MESH_INDEX_2D(0, i);
				output[counter++] = MESH_INDEX_2D(0, i + 1);
				for (int j = 0; j < width; j++)
				{
					output[counter++] = MESH_INDEX_2D(j + 1, i);
					output[counter++] = MESH_INDEX_2D(j + 1, i + 1);
				}
			}
			else
			{
				output[counter++] = MESH_INDEX_2D(width, i);
				output[counter++] = MESH_INDEX_2D(width, i + 1);
				for (int j = width - 1; j >= 0; j--)
				{
					output[counter++] = MESH_INDEX_2D(j, i);
					output[counter++] = MESH_INDEX_2D(j, i + 1);
				}
			}

			reverse = !reverse;
		}

		return counter;
	}


	// Generate boundary mesh for a patch. Return the number of generated indices
	int generateBoundaryMesh(int nMeshDim, int left_degree, int right_degree, int bottom_degree, int top_degree, RECT vert_rect, DWORD* output)
	{
		// Triangle list for bottom boundary
		int counter = 0;
		int width = vert_rect.right - vert_rect.left;

		if (bottom_degree > 0)
		{
			int b_step = width / bottom_degree;

			for (int i = 0; i < width; i += b_step)
			{
				output[counter++] = MESH_INDEX_2D(i, 0);
				output[counter++] = MESH_INDEX_2D(i + b_step / 2, 1);
				output[counter++] = MESH_INDEX_2D(i + b_step, 0);

				for (int j = 0; j < b_step / 2; j++)
				{
					if (i == 0 && j == 0 && left_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(i, 0);
					output[counter++] = MESH_INDEX_2D(i + j, 1);
					output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
				}

				for (int j = b_step / 2; j < b_step; j++)
				{
					if (i == width - b_step && j == b_step - 1 && right_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(i + b_step, 0);
					output[counter++] = MESH_INDEX_2D(i + j, 1);
					output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
				}
			}
		}

		// Right boundary
		int height = vert_rect.top - vert_rect.bottom;

		if (right_degree > 0)
		{
			int r_step = height / right_degree;

			for (int i = 0; i < height; i += r_step)
			{
				output[counter++] = MESH_INDEX_2D(width, i);
				output[counter++] = MESH_INDEX_2D(width - 1, i + r_step / 2);
				output[counter++] = MESH_INDEX_2D(width, i + r_step);

				for (int j = 0; j < r_step / 2; j++)
				{
					if (i == 0 && j == 0 && bottom_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(width, i);
					output[counter++] = MESH_INDEX_2D(width - 1, i + j);
					output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
				}

				for (int j = r_step / 2; j < r_step; j++)
				{
					if (i == height - r_step && j == r_step - 1 && top_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(width, i + r_step);
					output[counter++] = MESH_INDEX_2D(width - 1, i + j);
					output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
				}
			}
		}

		// Top boundary
		if (top_degree > 0)
		{
			int t_step = width / top_degree;

			for (int i = 0; i < width; i += t_step)
			{
				output[counter++] = MESH_INDEX_2D(i, height);
				output[counter++] = MESH_INDEX_2D(i + t_step / 2, height - 1);
				output[counter++] = MESH_INDEX_2D(i + t_step, height);

				for (int j = 0; j < t_step / 2; j++)
				{
					if (i == 0 && j == 0 && left_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(i, height);
					output[counter++] = MESH_INDEX_2D(i + j, height - 1);
					output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
				}

				for (int j = t_step / 2; j < t_step; j++)
				{
					if (i == width - t_step && j == t_step - 1 && right_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(i + t_step, height);
					output[counter++] = MESH_INDEX_2D(i + j, height - 1);
					output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
				}
			}
		}

		// Left boundary
		if (left_degree > 0)
		{
			int l_step = height / left_degree;

			for (int i = 0; i < height; i += l_step)
			{
				output[counter++] = MESH_INDEX_2D(0, i);
				output[counter++] = MESH_INDEX_2D(1, i + l_step / 2);
				output[counter++] = MESH_INDEX_2D(0, i + l_step);

				for (int j = 0; j < l_step / 2; j++)
				{
					if (i == 0 && j == 0 && bottom_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(0, i);
					output[counter++] = MESH_INDEX_2D(1, i + j);
					output[counter++] = MESH_INDEX_2D(1, i + j + 1);
				}

				for (int j = l_step / 2; j < l_step; j++)
				{
					if (i == height - l_step && j == l_step - 1 && top_degree > 0)
					{
						continue;
					}

					output[counter++] = MESH_INDEX_2D(0, i + l_step);
					output[counter++] = MESH_INDEX_2D(1, i + j);
					output[counter++] = MESH_INDEX_2D(1, i + j + 1);
				}
			}
		}

		return counter;
	}

	// Dimension of displacement map
	float CD3D11OceanSimulator::sUpperGridCoverage = 64.0f;
	float CD3D11OceanSimulator::sPatchLength = 2000;
	int CD3D11OceanSimulator::sDisplaceMapDim = 512;

	CD3D11OceanSimulator::CD3D11OceanSimulator()
	{
		// Grid side length * 2
		shadingData.g_TexelLength_x2 = sPatchLength / sDisplaceMapDim * 2;;
		// Color
		shadingData.g_SkyColor = Vec3F(0.38f, 0.45f, 0.56f);;
		shadingData.g_WaterbodyColor = Vec3F(0.07f, 0.15f, 0.2f);;
		// Texcoord
		shadingData.g_UVScale = 1.0f / sPatchLength;
		shadingData.g_UVOffset = 0.5f / sDisplaceMapDim;
		// Perlin
		shadingData.g_PerlinSize = 1.0f;
		shadingData.g_PerlinAmplitude = Vec3F(35, 42, 57);;
		shadingData.g_PerlinGradient = Vec3F(1.4f, 1.6f, 2.2f);;
		shadingData.g_PerlinOctave = Vec3F(1.12f, 0.59f, 0.23f);;
		// Multiple reflection workaround
		shadingData.g_BendParam = Vec3F(0.1f, -0.4f, 0.2f);;
		// Sun streaks
		shadingData.g_SunColor = Vec3F(1.0f, 1.0f, 0.6f);;
		shadingData.g_SunDir = Vec3F(0.936016f, -0.343206f, 0.0780013f);;
		shadingData.g_Shineness = 400.0f;
	}

	CD3D11OceanSimulator::~CD3D11OceanSimulator()
	{
		//SAFE_RELEASE(mpBufferFloat2H0);
		//SAFE_RELEASE(mpUAVH0);
		//SAFE_RELEASE(mpSRVH0);

		//SAFE_RELEASE(mpBufferFloatOmega);
		//SAFE_RELEASE(mpUAVOmega);
		//SAFE_RELEASE(mpSRVOmega);

		//SAFE_RELEASE(mpBufferFloat2Ht);
		//SAFE_RELEASE(mpUAVHt);
		//SAFE_RELEASE(mpSRVHt);

		//SAFE_RELEASE(mpBufferFloatDxyz);
		//SAFE_RELEASE(mpUAVDxyz);
		//SAFE_RELEASE(mpSRVDxyz);
		DEV_RELEASE(mpQuadVB);

		WL_DELETE(mpOceanSimulatorShader, Instance);
		WL_DELETE(mpQuadVS, Instance);
		WL_DELETE(mpUpdateDisplacementPS, Instance);
		WL_DELETE(mpGenGradientFoldingPS, Instance);

		WL_DELETE(m_pUAV_H0, SurfaceView);
		WL_DELETE(m_pUAV_Ht, SurfaceView);
		WL_DELETE(m_pUAV_Omega, SurfaceView);
		WL_DELETE(m_pUAV_Dxyz, SurfaceView);
		WL_DELETE(mpDisplacement, SurfaceView);
		WL_DELETE(mpGradientMap, SurfaceView);
		WL_DELETE(mpOceanRS, RenderState);
		//SAFE_RELEASE(mpImmutableCB);
		//SAFE_RELEASE(mpPerFrameCB);

		cleanupRenderResource();

		mMeshInstancesPool.insert(mMeshInstancesPool.end(), mMeshInstances.begin(), mMeshInstances.end());
		for( auto item : mMeshInstancesPool)
		{
			SafeDelete(item);
		}
		mMeshInstancesPool.clear();
#ifdef CS_DEBUG_BUFFER
		DEV_RELEASE(mpDebugBuffer);
#endif
	}

	void CD3D11OceanSimulator::updateDisplacementMap(float time)
	{
		auto pContext = GEngine->getImmediateContext();
		GEngine->setRasterizerState(pContext, mpOceanRS);

		mpOceanSimulatorShader->setShader(pContext, CS_FLAG);
		m_pUAV_H0->setCShaderResource(pContext, 0);
		m_pUAV_Omega->setCShaderResource(pContext, 1);
		m_pUAV_Ht->setCSUAVSurface(pContext, 0);

		float per_frame_data[3] = { time * mParam.time_scale, mParam.choppy_scale, mParam.dmap_dim / mParam.patch_length };
	
		mpOceanSimulatorShader->updateCSConstBuffer("PerFrameCB", per_frame_data);
		mpOceanSimulatorShader->commitCSToGpu(pContext);

		UINT group_count_x = (mParam.dmap_dim + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
		UINT group_count_y = (mParam.dmap_dim + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
		pContext->Dispatch(group_count_x, group_count_y, 1);


		////GEngine->getImmediateContext()->CopyResource(mpDebugBuffer, m_pUAV_Ht->getBufferPtr());
		////D3D11_MAPPED_SUBRESOURCE mapped_res;
		////GEngine->getImmediateContext()->Map(mpDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped_res);
	
		////Vec2F* v = (Vec2F*)mapped_res.pData;

		////
		////GEngine->getImmediateContext()->Unmap(mpDebugBuffer, 0);


		m_pUAV_Ht->clearCSUAVSurface(pContext);
		m_pUAV_H0->clearCSShaderResource(pContext);
		m_pUAV_Omega->clearCSShaderResource(pContext);

		fft_512x512_c2c(m_pUAV_Dxyz->getUnorderedAccessViewPtr(), m_pUAV_Dxyz->getShaderResourceView(), m_pUAV_Ht->getShaderResourceView());
		//D3D11_VIEWPORT new_vp = { 0, 0, (float)mParam.dmap_dim, (float)mParam.dmap_dim, 0.0f, 1.0f };
		//pContext->RSSetViewports(1, &new_vp);

		mpDisplacement->setRenderSurface(pContext);

		// VS & PS
		mpQuadVS->setShader(pContext, VS_FLAG); //pContext->VSSetShader(mpQuadVS, NULL, 0);
	
		mpUpdateDisplacementPS->setShader(pContext, PS_FLAG); //pContext->PSSetShader(m_pUpdateDisplacementPS, NULL, 0);

		// Constants

		//ID3D11Buffer* ps_cbs[2] = { m_pImmutableCB, m_pPerFrameCB };
		//pContext->PSSetConstantBuffers(0, 2, ps_cbs);
		// 
		mpOceanSimulatorShader->setConstBuffer(pContext, "ImmutableCB", 0, PS_FLAG);
		mpOceanSimulatorShader->setConstBuffer(pContext, "PerFrameCB", 1, PS_FLAG);
		// 
		// Buffer resources
		//ID3D11ShaderResourceView* ps_srvs[1] = { m_pSRV_Dxyz };
		m_pUAV_Dxyz->setPShaderResource(pContext, 0); //pContext->PSSetShaderResources(0, 1, ps_srvs);

		// IA setup
		ID3D11Buffer* vbs[1] = { mpQuadVB };
		UINT strides[1] = { sizeof(D3DXVECTOR4) };
		UINT offsets[1] = { 0 };
		pContext->IASetVertexBuffers(0, 1, &vbs[0], &strides[0], &offsets[0]);
		mpQuadVS->setInputLayout(pContext);
		pContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(mpQuadVS->getTopology()));
		
		// Perform draw call
		pContext->Draw(4, 0);

		mpGradientMap->setRenderSurface(pContext);
		mpQuadVS->setShader(pContext, VS_FLAG); //pContext->VSSetShader(mpQuadVS, NULL, 0);
		mpGenGradientFoldingPS->setShader(pContext, PS_FLAG);
		mpDisplacement->setPShaderResource(pContext, 0);

		mpMaterial->setSampler(pContext, 0, 0);

		pContext->Draw(4, 0);
		
#if 0	
		//xf
		pContext->GenerateMips(mpGradientMap->getShaderResourceView());
#endif

		GEngine->restoreDeviceRenderTargets();


		//mpOceanSimulatorShader->updateCSConstBuffer("",);
		//GEngine->getImmediateContext()->CSSetShader(m_pUpdateSpectrumCS, NULL, 0);
	// ---------------------------- H(0) -> H(t), D(x, t), D(y, t) --------------------------------
// Compute shader
		//pContext->CSSetShader(mpUpdateSpectrumCS, NULL, 0);

		//// Buffers
		//ID3D11ShaderResourceView* cs0_srvs[2] = { mpSRVH0, mpSRVOmega };
		//pContext->CSSetShaderResources(0, 2, cs0_srvs);

		//ID3D11UnorderedAccessView* cs0_uavs[1] = { mpUAVHt };
		//pContext->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (UINT*)(&cs0_uavs[0]));
#ifdef CS_DEBUG_BUFFER
		{
			//GEngine->getImmediateContext()->CopyResource(mpDebugBuffer, m_pUAV_Dxyz->getBufferPtr());
			//D3D11_MAPPED_SUBRESOURCE mapped_res;
			//GEngine->getImmediateContext()->Map(mpDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped_res);

			//// set a break point below, and drag MappedResource.pData into in your Watch window
			//// and cast it as (float*)

			//// Write to disk
			//Vec2F* v = (Vec2F*)mapped_res.pData;

			//FILE* fp = fopen(".\\tmp\\Ht_raw.dat", "wb");
			//fwrite(v, 512 * 512 * sizeof(float) * 2 * 3, 1, fp);
			//fclose(fp);

			//GEngine->getImmediateContext()->Unmap(mpDebugBuffer, 0);
		}
#endif
	}

	bool CD3D11OceanSimulator::initialise()
	{
		sPatchLength = mParam.patch_length;
		sDisplaceMapDim = mParam.dmap_dim;

//		mpGraphicDevice = GEngine->getGraphicsDevice();

		int height_map_size = (mParam.dmap_dim + 4) * (mParam.dmap_dim + 1);
		Vec2F* h0_data = WL_NEW_ARRAY(Vec2F, height_map_size * sizeof(Vec2F));
		float* omega_data = WL_NEW_ARRAY(float, height_map_size * sizeof(float));
		initHeightMap(h0_data, omega_data);

		UINT float2_stride = 2 * sizeof(float);
		int hmap_dim = mParam.dmap_dim;
		int input_full_size = (hmap_dim + 4) * (hmap_dim + 1);
		int input_half_size = hmap_dim * hmap_dim;
		int output_size = hmap_dim * hmap_dim;

		// For filling the buffer with zeroes.
		char* pZeroData = WL_NEW_ARRAY(char, 3 * output_size * sizeof(float) * 2);
		memset(pZeroData, 0, 3 * output_size * sizeof(float) * 2);
		mpDisplacement = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
		if (nullptr != mpDisplacement)
		{
			mpDisplacement->createRenderSurface(hmap_dim, hmap_dim, FORMAT_R32G32B32A32_FLOAT, 0, BIND_SHADER_RESOURCE | BIND_RENDER_TARGET, RESOURCE_MISC_GENERATE_MIPS);

			m_pUAV_H0 = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
			m_pUAV_Ht = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
			m_pUAV_Omega = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
			m_pUAV_Dxyz = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));

			if (nullptr != m_pUAV_H0 ||
				nullptr != m_pUAV_Ht ||
				nullptr != m_pUAV_Omega ||
				nullptr != m_pUAV_Dxyz)
			{
				UINT float2_stride = 2 * sizeof(float);
				bool bResult = false;
				bResult = m_pUAV_H0->createUAVBuffer(h0_data, input_full_size * float2_stride, float2_stride);
				bResult = m_pUAV_Ht->createUAVBuffer(pZeroData, 3 * input_half_size * float2_stride, float2_stride);
				bResult = m_pUAV_Omega->createUAVBuffer(omega_data, input_full_size * sizeof(float), sizeof(float));
				bResult = m_pUAV_Dxyz->createUAVBuffer(pZeroData, 3 * output_size * float2_stride, float2_stride);
			}
		}
		WL_DELETE_ARRAY(pZeroData);
		WL_DELETE_ARRAY(h0_data);
		WL_DELETE_ARRAY(omega_data);
		//// This value should be (hmap_dim / 2 + 1) * hmap_dim, but we use full sized buffer here for simplicity.
		//createBufferAndUAV(h0_data, input_full_size * float2_stride, float2_stride, &mpBufferFloat2H0, &mpUAVH0, &mpSRVH0);

		//// Put H(t), Dx(t) and Dy(t) into one buffer because CS4.0 allows only 1 UAV at a time
		//createBufferAndUAV(pZeroData, 3 * input_half_size * float2_stride, float2_stride, &mpBufferFloat2Ht, &mpUAVHt, &mpSRVHt);

		//// omega
		//createBufferAndUAV(omega_data, input_full_size * sizeof(float), sizeof(float), &mpBufferFloatOmega, &mpUAVOmega, &mpSRVOmega);

		//// Notice: The following 3 should be real number data. But here we use the complex numbers and C2C FFT
		//// due to the CS4.0 restriction.
		//// Put Dz, Dx and Dy into one buffer because CS4.0 allows only 1 UAV at a time
		//createBufferAndUAV(pZeroData, 3 * output_size * float2_stride, float2_stride, &mpBufferFloatDxyz, &mpUAVDxyz, &mpSRVDxyz);

		//mDisplacementMap.initialiseTexture(mParam.dmap_dim, mParam.dmap_dim, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, D3D11_USAGE_DEFAULT);
		//mGradientMap.initialiseTexture2D(mParam.dmap_dim, mParam.dmap_dim, FORMAT_R16G16B16A16_FLOAT, BIND_SHADER_RESOURCE | BIND_RENDER_TARGET, 0, USAGE_DEFAULT);

		mpGradientMap = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
		if (nullptr != mpGradientMap)
		{
			mpGradientMap->createRenderSurface(hmap_dim, hmap_dim, FORMAT_R16G16B16A16_FLOAT, 0, BIND_SHADER_RESOURCE | BIND_RENDER_TARGET, 0);
		}
		//mSRV_Perlin = *dynamic_cast<CTexture*>(GEngine->createResource("Resource/Texture/perlinNoise.dds", ResourceType::Texture));
		// Quad vertex buffer
		D3D11_BUFFER_DESC vb_desc;
		vb_desc.ByteWidth = 4 * sizeof(Vec4F);
		vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vb_desc.CPUAccessFlags = 0;
		vb_desc.MiscFlags = 0;

		float quad_verts[] =
		{
			-1, -1, 0, 1,
			-1,  1, 0, 1,
			 1, -1, 0, 1,
			 1,  1, 0, 1,
		};
		D3D11_SUBRESOURCE_DATA init_data;
		init_data.pSysMem = &quad_verts[0];
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		GEngine->getDevice()->createBuffer(&vb_desc, &init_data, &mpQuadVB);

		// Constant buffers
		UINT actual_dim = mParam.dmap_dim;
		UINT input_width = actual_dim + 4;
		// We use full sized data here. The value "output_width" should be actual_dim/2+1 though.
		UINT output_width = actual_dim;
		UINT output_height = actual_dim;
		UINT dtx_offset = actual_dim * actual_dim;
		UINT dty_offset = actual_dim * actual_dim * 2;
		UINT immutable_consts[] = { actual_dim, input_width, output_width, output_height, dtx_offset, dty_offset };
		SSUBRESOURCE_DATA init_cb0 = { &immutable_consts[0], 0, 0 };
		
		CShader* pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			if (pShader->createComputeShader("Resource/Shader/oceanSimulatorCS.hlsl", "UpdateSpectrumCS", "cs_5_0"))
			{
				pShader->addCSValue("ImmutableCB", sizeof(immutable_consts), 0, 0, USAGE_DYNAMIC, BIND_CONSTANT_BUFFER, CPU_ACCESS_WRITE, &init_cb0);
				pShader->addCSValue("PerFrameCB", PAD16(sizeof(float) * 3), 1, 0, USAGE_DYNAMIC, BIND_CONSTANT_BUFFER, CPU_ACCESS_WRITE, nullptr);
				mpOceanSimulatorShader = WL_NEW(CShaderInstance, Instance)(pShader);
			}
			WL_DECREASE(pShader);
		}

		pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			std::vector<INPUT_ELEMENT_DESC> desc;
			INPUT_ELEMENT_DESC quad_layout_desc =
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
			};
			desc.push_back(quad_layout_desc);

			if (pShader->createVextexShader("Resource/Shader/oceanVsPs.hlsl", "QuadVS", "vs_4_0", desc))
			{
				pShader->setTopology(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				mpQuadVS = WL_NEW(CShaderInstance, Instance)(pShader);
			}
			WL_DECREASE(pShader);
		}

		pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			if (pShader->createPixelShader("Resource/Shader/oceanVsPs.hlsl", "UpdateDisplacementPS", "ps_4_0"))
			{
				mpUpdateDisplacementPS = WL_NEW(CShaderInstance, Instance)(pShader);
			}
			WL_DECREASE(pShader);
		}

		pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			if (pShader->createPixelShader("Resource/Shader/oceanVsPs.hlsl", "GenGradientFoldingPS", "ps_4_0"))
			{
				mpGenGradientFoldingPS = WL_NEW(CShaderInstance, Instance)(pShader); 
			}
			WL_DECREASE(pShader);
		}


		fft512x512_create_plan(3);

		initRenderResource();
#ifdef CS_DEBUG_BUFFER
		D3D11_BUFFER_DESC buf_desc;

		buf_desc.ByteWidth = 3 * input_half_size * float2_stride;
		buf_desc.Usage = D3D11_USAGE_STAGING;
		buf_desc.BindFlags = 0;
		buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buf_desc.StructureByteStride = float2_stride;
		
		GEngine->getDevice()->createBuffer(&buf_desc, nullptr, &mpDebugBuffer);
		assert(mpDebugBuffer);
#endif
		return true;
	}

	void CD3D11OceanSimulator::initRenderResource()
	{
		sPatchLength = mParam.patch_length;
		sDisplaceMapDim = mParam.dmap_dim;
		auto windDir = mParam.wind_dir;

		createSurfaceMesh();
		createFresnelMap();

		// Samplers
		D3D11_SAMPLER_DESC sam_desc;
		sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_desc.MipLODBias = 0;
		sam_desc.MaxAnisotropy = 1;
		sam_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sam_desc.BorderColor[0] = 1.0f;
		sam_desc.BorderColor[1] = 1.0f;
		sam_desc.BorderColor[2] = 1.0f;
		sam_desc.BorderColor[3] = 1.0f;
		sam_desc.MinLOD = 0;
		sam_desc.MaxLOD = FLT_MAX;
		//GEngine->getGraphicsDevice()->CreateSamplerState(&sam_desc, &mpHeightSampler);
		//assert(mpHeightSampler);
		//mDisplacementMap.setSampleState(mpHeightSampler);

		sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		GEngine->getGraphicsDevice()->CreateSamplerState(&sam_desc, &mpCubeSampler);
		assert(mpCubeSampler);

		D3D11_BLEND_DESC blend_desc;
		memset(&blend_desc, 0, sizeof(D3D11_BLEND_DESC));
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		GEngine->getGraphicsDevice()->CreateBlendState(&blend_desc, &mpBStateTransparent);
		assert(mpBStateTransparent);

		auto pSampler = mpMaterial->getSampler(3);
		mFresnel.setSampleState(pSampler);

		SRenderStateDesc renderStateDesc;
		//memset(&renderStateDesc, 0, sizeof(RenderStateDesc));
		renderStateDesc.bCreateBlend = false;
		//renderStateDesc.bCreateDepthStencil = true;
		renderStateDesc.bCreateRasterizer = true;

		renderStateDesc.rasterizerDesc.CullMode = CULL_NONE;
		renderStateDesc.rasterizerDesc.FillMode = FILL_SOLID;
		renderStateDesc.rasterizerDesc.DepthClipEnable = TRUE;
			
		mpOceanRS = Dev->createRenderState(renderStateDesc);
		assert(mpBStateTransparent);

	}

	void CD3D11OceanSimulator::cleanupRenderResource()
	{
		WL_DECREASE(mpMesh);

		DEV_RELEASE(mpCubeSampler);
		DEV_RELEASE(mpBStateTransparent);
	}

	void CD3D11OceanSimulator::createSurfaceMesh()
	{
		// --------------------------------- Vertex Buffer -------------------------------
		int num_verts = (sMeshDim + 1) * (sMeshDim + 1);
		OceanVertex* pV = WL_NEW_ARRAY(OceanVertex, num_verts);
		assert(pV);


		for (int i = 0; i <= sMeshDim; i++)
		{
			for (int j = 0; j <= sMeshDim; j++)
			{
				pV[i * (sMeshDim + 1) + j].index_x = (float)j;
				pV[i * (sMeshDim + 1) + j].index_y = (float)i;
			}
		}

		mpMesh = GEngine->createResource<CMesh>(Mesh);
		mpMesh->fillVertexBuffer(pV, sizeof(OceanVertex) * num_verts, sizeof(OceanVertex), 0, D3D11_USAGE_IMMUTABLE);
		WL_DELETE_ARRAY(pV);

		// --------------------------------- Index Buffer -------------------------------
		// The index numbers for all mesh LODs (up to 256x256)
		const int index_size_lookup[] = { 0, 0, 4284, 18828, 69444, 254412, 956916, 3689820, 14464836 };

		memset(&mMeshPatterns[0][0][0][0][0], 0, sizeof(mMeshPatterns));

		mLods = 0;
		for (int i = sMeshDim; i > 1; i >>= 1)
		{
			mLods++;
		}
		// Generate patch meshes. Each patch contains two parts: the inner mesh which is a regular
		// grids in a triangle strip. The boundary mesh is constructed w.r.t. the edge degrees to
		// meet water-tight requirement.
		DWORD* index_array = WL_NEW_ARRAY(DWORD, index_size_lookup[mLods]);
		assert(index_array);

		int offset = 0;
		int level_size = sMeshDim;

		// Enumerate patterns
		for (int level = 0; level <= mLods - 2; level++)
		{
			int left_degree = level_size;

			for (int left_type = 0; left_type < 3; left_type++)
			{
				int right_degree = level_size;

				for (int right_type = 0; right_type < 3; right_type++)
				{
					int bottom_degree = level_size;

					for (int bottom_type = 0; bottom_type < 3; bottom_type++)
					{
						int top_degree = level_size;

						for (int top_type = 0; top_type < 3; top_type++)
						{
							SQuadRenderParam* pattern = &mMeshPatterns[level][left_type][right_type][bottom_type][top_type];

							// Inner mesh (triangle strip)
							RECT inner_rect;
							inner_rect.left = (left_degree == level_size) ? 0 : 1;
							inner_rect.right = (right_degree == level_size) ? level_size : level_size - 1;
							inner_rect.bottom = (bottom_degree == level_size) ? 0 : 1;
							inner_rect.top = (top_degree == level_size) ? level_size : level_size - 1;

							int num_new_indices = generateInnerMesh(inner_rect, index_array + offset, sMeshDim);

							pattern->inner_start_index = offset;
							pattern->num_inner_verts = (level_size + 1) * (level_size + 1);
							pattern->num_inner_faces = num_new_indices - 2;
							offset += num_new_indices;

							// Boundary mesh (triangle list)
							int l_degree = (left_degree == level_size) ? 0 : left_degree;
							int r_degree = (right_degree == level_size) ? 0 : right_degree;
							int b_degree = (bottom_degree == level_size) ? 0 : bottom_degree;
							int t_degree = (top_degree == level_size) ? 0 : top_degree;

							RECT outer_rect = { 0, level_size, level_size, 0 };
							num_new_indices = generateBoundaryMesh(sMeshDim, l_degree, r_degree, b_degree, t_degree, outer_rect, index_array + offset);

							pattern->boundary_start_index = offset;
							pattern->num_boundary_verts = (level_size + 1) * (level_size + 1);
							pattern->num_boundary_faces = num_new_indices / 3;
							offset += num_new_indices;

							top_degree /= 2;
						}
						bottom_degree /= 2;
					}
					right_degree /= 2;
				}
				left_degree /= 2;
			}
			level_size /= 2;
		}

		assert(offset == index_size_lookup[mLods]);

		mpMesh->fillIndexBuffer(index_array, index_size_lookup[mLods] * sizeof(DWORD), sizeof(DWORD), 0, D3D11_USAGE_IMMUTABLE);
		WL_DELETE_ARRAY(index_array);
	}

	void CD3D11OceanSimulator::createFresnelMap()
	{
		DWORD* buffer = WL_NEW_ARRAY(DWORD, FRESNEL_TEX_SIZE);
		for (int i = 0; i < FRESNEL_TEX_SIZE; i++)
		{
			float cos_a = i / (FLOAT)FRESNEL_TEX_SIZE;
			// Using water's refraction index 1.33
			DWORD fresnel = (DWORD)(D3DXFresnelTerm(cos_a, 1.33f) * 255);

			DWORD sky_blend = (DWORD)(powf(1 / (1 + cos_a), g_SkyBlending) * 255);

			buffer[i] = (sky_blend << 8) | fresnel;
		}

		mFresnel.initialiseTexture1D(FRESNEL_TEX_SIZE, FORMAT_R8G8B8A8_UNORM, BIND_SHADER_RESOURCE, 0, USAGE_IMMUTABLE, buffer);
		WL_DELETE_ARRAY(buffer);
	}

	SMeshInstanceInfo* CD3D11OceanSimulator::getMeshInstance()
	{
		SMeshInstanceInfo* pMeshInstance = nullptr;
		if (mMeshInstancesPool.size() > 0 )
		{
			pMeshInstance = mMeshInstancesPool.front();
			mMeshInstancesPool.pop_front();
		}
		else
		{
			pMeshInstance = WL_NEW(SMeshInstanceInfo, Default); 
			pMeshInstance->name = "OceanTile";
			pMeshInstance->pMeshInstance = WL_NEW(CMeshInstance, Instance)(mpMesh); 
			auto pMaterialInstance = WL_NEW(CMaterialInstance, Instance)(mpMaterial); 
	
			pMaterialInstance->setVSParamByName("ShadingData", &shadingData);
			pMaterialInstance->setPSParamByName("ShadingData", &shadingData);
			mpDisplacement->getTexturePtr()->setName("Displacement");

			pMaterialInstance->bindTexture(TextureUseType::INDEX0, mpDisplacement->getTexturePtr(), ShaderBufferType::VS);
			pMaterialInstance->bindTexture(TextureUseType::INDEX2, mpGradientMap->getTexturePtr(), ShaderBufferType::PS);
			pMaterialInstance->bindTexture(TextureUseType::INDEX3, &mFresnel, ShaderBufferType::PS);
			pMeshInstance->pMaterialInstance = pMaterialInstance;
		}
		return pMeshInstance;
	}

	void CD3D11OceanSimulator::setMaterial(CMaterial* pMaterial)
	{
		mpMaterial = pMaterial;
	}

	void CD3D11OceanSimulator::updateNodes(UINT32 nElapsedTime)
	{
		static UINT32 nTime = 0;
		nTime += nElapsedTime;
		float dt = nTime / 1000.0f;

		updateDisplacementMap(dt);
		// Matrices
		SMatrix44 matView = SMatrix44(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * GEngine->getMainCameraEntity()->getViewMatrix();
		SMatrix44 matProj = GEngine->getMainCameraEntity()->getProjectMatrix();
	

		// Constant g_PerlinSpeed need to be adjusted mannually
		Vec2F perlin_move = -mParam.wind_dir * dt * g_PerlinSpeed;
		SMatrix44 matVP = matView * matProj;
		for (auto& node : mRenderNodes)
		{
			if (!isLeaf(node))
			{
				continue;
			}
			int level_size = sMeshDim;
			for (int lod = 0; lod < node.lod; lod++)
			{
				level_size >>= 1;
			}
			// Matrices and constants
			SConst_Per_Call call_consts;

			// Expand of the local coordinate to world space patch size
			SMatrix44 matScale;
			Math::matrixIdentity(&matScale);
			Math::matrixScale(&matScale, node.length / level_size, node.length / level_size, 0);
			Math::matrixTranspose(&call_consts.g_matLocal, &matScale);

			// WVP matrix
			SMatrix44 matWorld;
			Math::matrixIdentity(&matWorld);
			Math::matrixTranslation(&matWorld, node.bottom_left.x, node.bottom_left.y, 0);
			SMatrix44 matWVP = matWorld * matVP;
			Math::matrixTranspose(&call_consts.g_matWorldViewProj, &matWVP);

			// Texcoord for perlin noise
			Vec2F uv_base = node.bottom_left / mParam.patch_length * g_PerlinSize;
			call_consts.g_UVBase = uv_base;


			call_consts.g_PerlinMovement = perlin_move;

			// Eye point
			SMatrix44 matInvWV = matWorld * matView;
			Math::matrixInverse(&matInvWV, &matInvWV);
			Vec3F vLocalEye(0, 0, 0);
			vLocalEye =	Math::matrixMulVec3(matInvWV, vLocalEye);
			call_consts.g_LocalEye = vLocalEye;

			// Consts

			float per_frame_data[3];
			// g_Time
			per_frame_data[0] = dt * mParam.time_scale;
			// g_ChoppyScale
			per_frame_data[1] = mParam.choppy_scale;
			// g_GridLen
			per_frame_data[2] = mParam.dmap_dim / mParam.patch_length;
		//	node.pMeshInstanceInfo[0]->pMaterialInstance->bindTexture()
			if (nullptr != node.pMeshInstanceInfo[0])
			{
				node.pMeshInstanceInfo[0]->pMaterialInstance->setVSParamByName("PerCallData", &call_consts);
				node.pMeshInstanceInfo[0]->pMaterialInstance->setPSParamByName("PerCallData", &call_consts);
				node.pMeshInstanceInfo[0]->pMaterialInstance->setCSParamByName("PerFrameCB", per_frame_data);
			}
			if (nullptr != node.pMeshInstanceInfo[1])
			{
				node.pMeshInstanceInfo[1]->pMaterialInstance->setVSParamByName("PerCallData", &call_consts);
				node.pMeshInstanceInfo[1]->pMaterialInstance->setPSParamByName("PerCallData", &call_consts);
				node.pMeshInstanceInfo[1]->pMaterialInstance->setCSParamByName("PerFrameCB", per_frame_data);
			}
		}
	}

	void CD3D11OceanSimulator::radix008A(ID3D11UnorderedAccessView* pUAV_Dst,
		ID3D11ShaderResourceView* pSRV_Src,
		UINT32 thread_count,
		UINT32 istride)
	{
		// Setup execution configuration
		UINT grid = thread_count / COHERENCY_GRANULARITY;
		// Buffers
		ID3D11ShaderResourceView* cs_srvs[1] = { pSRV_Src };
		m_fft_plan.pContext->CSSetShaderResources(0, 1, cs_srvs);

		ID3D11UnorderedAccessView* cs_uavs[1] = { pUAV_Dst };
		m_fft_plan.pContext->CSSetUnorderedAccessViews(0, 1, cs_uavs, (UINT*)(&cs_uavs[0]));

		// Shader
		if (istride > 1)
		{
			m_fft_plan.pRadix008A_CS->setShader(m_fft_plan.pContext, CS_FLAG);
			//m_fft_plan.pContext->CSSetShader(m_fft_plan.pRadix008A_CS, NULL, 0);
		}

		else
		{
			m_fft_plan.pRadix008A_CS2->setShader(m_fft_plan.pContext, CS_FLAG);
			//m_fft_plan.pContext->CSSetShader(m_fft_plan.pRadix008A_CS2, NULL, 0);
		}
		// Execute
		m_fft_plan.pContext->Dispatch(grid, 1, 1);

		// Unbind resource
		cs_srvs[0] = nullptr;
		m_fft_plan.pContext->CSSetShaderResources(0, 1, cs_srvs);

		cs_uavs[0] = nullptr;
		m_fft_plan.pContext->CSSetUnorderedAccessViews(0, 1, cs_uavs, (UINT*)(&cs_uavs[0]));
	}

	void CD3D11OceanSimulator::fft_512x512_c2c(ID3D11UnorderedAccessView* pUAV_Dst,
		ID3D11ShaderResourceView* pSRV_Dst,
		ID3D11ShaderResourceView* pSRV_Src)
	{
		UINT32 istride = 512 * 512 / 8;
		const UINT thread_count = m_fft_plan.slices * (512 * 512) / 8;
		ID3D11UnorderedAccessView* pUAV_Tmp = m_fft_plan.pTmpUAV->getUnorderedAccessViewPtr();
		ID3D11ShaderResourceView* pSRV_Tmp = m_fft_plan.pTmpUAV->getShaderResourceView();

		m_fft_plan.mRadix008A_CB[0].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Tmp, pSRV_Src, thread_count, istride);
		
		istride /= 8;
		m_fft_plan.mRadix008A_CB[1].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Dst, pSRV_Tmp, thread_count, istride);

		istride /= 8;
		m_fft_plan.mRadix008A_CB[2].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Tmp, pSRV_Dst, thread_count, istride);

		istride /= 8;
		m_fft_plan.mRadix008A_CB[3].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Dst, pSRV_Tmp, thread_count, istride);

		istride /= 8;
		m_fft_plan.mRadix008A_CB[4].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Tmp, pSRV_Dst, thread_count, istride);

		istride /= 8;
		m_fft_plan.mRadix008A_CB[5].setCSConstBuffer(m_fft_plan.pContext, 0);
		radix008A(pUAV_Dst, pSRV_Tmp, thread_count, istride);
	}

	void CD3D11OceanSimulator::fft512x512_create_plan(UINT32 slices)
	{
		m_fft_plan.slices = slices;
		m_fft_plan.pContext = GEngine->getImmediateContext();
		m_fft_plan.pTmpUAV = dynamic_cast<CSurfaceView*>(GEngine->createResource(SurfaceView));
		if (nullptr != m_fft_plan.pTmpUAV)
		{
			create_cbuffers_512x512();
			m_fft_plan.pTmpUAV->createUAVBuffer(nullptr, sizeof(float) * 2 * (512 * slices) * 512, sizeof(float) * 2);
		}
		CShader* pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			pShader->createComputeShader("Resource/Shader/fft_512x512_c2c.hlsl", "Radix008A_CS", "cs_4_0");
			m_fft_plan.pRadix008A_CS = WL_NEW(CShaderInstance, Instance)(pShader);
			WL_DECREASE(pShader);
		}

		pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		if (nullptr != pShader)
		{
			pShader->setDevice(GEngine->getDevice());
			pShader->createComputeShader("Resource/Shader/fft_512x512_c2c.hlsl", "Radix008A_CS2", "cs_4_0");
			m_fft_plan.pRadix008A_CS2 = WL_NEW(CShaderInstance, Instance)(pShader);
			WL_DECREASE(pShader);
		}
	}

	void CD3D11OceanSimulator::create_cbuffers_512x512()
	{
		struct SCB_Structure
		{
			UINT thread_count;
			UINT ostride;
			UINT istride;
			UINT pstride;
			float phase_base;
		};

		// Buffer 0
		const UINT thread_count = m_fft_plan.slices * (512 * 512) / 8;
		UINT ostride = 512 * 512 / 8;
		UINT istride = ostride;
		double phase_base = -TWO_PI / (512.0 * 512.0);


		SCB_Structure cb_data_buf0 = { thread_count, ostride, istride, 512, (float)phase_base };
		//cb_data.pSysMem = &cb_data_buf0;
		m_fft_plan.mRadix008A_CB[0].createBuffer(&cb_data_buf0, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);
	

		istride /= 8;
		phase_base *= 8.0;

		SCB_Structure cb_data_buf1 = { thread_count, ostride, istride, 512, (float)phase_base };
		//cb_data.pSysMem = &cb_data_buf1;
		m_fft_plan.mRadix008A_CB[1].createBuffer(&cb_data_buf1, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);

		// Buffer 2
		istride /= 8;
		phase_base *= 8.0;

		SCB_Structure cb_data_buf2 = { thread_count, ostride, istride, 512, (float)phase_base };
		m_fft_plan.mRadix008A_CB[2].createBuffer(&cb_data_buf2, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);

		// Buffer 3
		istride /= 8;
		phase_base *= 8.0;
		ostride /= 512;

		SCB_Structure cb_data_buf3 = { thread_count, ostride, istride, 1, (float)phase_base };
		m_fft_plan.mRadix008A_CB[3].createBuffer(&cb_data_buf3, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);

		// Buffer 4
		istride /= 8;
		phase_base *= 8.0;

		SCB_Structure cb_data_buf4 = { thread_count, ostride, istride, 1, (float)phase_base };
		m_fft_plan.mRadix008A_CB[4].createBuffer(&cb_data_buf4, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);

		// Buffer 5
		istride /= 8;
		phase_base *= 8.0;

		SCB_Structure cb_data_buf5 = { thread_count, ostride, istride, 1, (float)phase_base };
		m_fft_plan.mRadix008A_CB[5].createBuffer(&cb_data_buf5, 32, 0, USAGE_DEFAULT, 0, BIND_CONSTANT_BUFFER, 0);
	}

}