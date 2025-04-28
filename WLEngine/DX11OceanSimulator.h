#pragma once
#include "Device.h"
#include "OceanSimulatorBase.h"
#include "Texture.h"
#include "MeshInstance.h"
#include "OceanModelInstance.h"
#include "DX11SurfaceView.h"
#include "FFT_Plan.h"

namespace WL
{
	class CD3D11OceanSimulator : public OceanSimulatorBase
	{
		friend class COceanEntity;
	public:
		CD3D11OceanSimulator();
		virtual	~CD3D11OceanSimulator();
		void updateDisplacementMap(float time);

	protected:
		bool initialise();
		void initRenderResource();
		void cleanupRenderResource();
		void createSurfaceMesh();
		void createFresnelMap();
		MeshInstanceInfo* getMeshInstance();
		void setMaterial(CMaterial* pMaterial);
		void updateNodes(UINT32 nElapsedTime);
	
	private:
		void radix008A(ID3D11UnorderedAccessView* pUAV_Dst, 
			ID3D11ShaderResourceView* pSRV_Src,
			UINT32 thread_count, 
			UINT32 istride);

		void fft_512x512_c2c(ID3D11UnorderedAccessView* pUAV_Dst,
			ID3D11ShaderResourceView* pSRV_Dst,
			ID3D11ShaderResourceView* pSRV_Src);

		void fft512x512_create_plan(UINT32 slices);
		void create_cbuffers_512x512();
	public:
		// Mesh grid dimension, must be 2^n. 4x4 ~ 256x256
		const static int sMeshDim = 128;
		// Draw distance = g_PatchLength * 2^g_FurthestCover
		const static int sFurthestCover = 8;
		// Dimension of displacement map
		static int sDisplaceMapDim;
		// Side length of square shaped mesh patch
		static float sPatchLength;
		// Subdivision thredshold. Any quad covers more pixels than this value needs to be subdivided.
		static float sUpperGridCoverage;
			
	protected:
		COceanModelInstance* mpModelInstance = nullptr;
		std::list<MeshInstanceInfo*>	mMeshInstances;
		std::list<MeshInstanceInfo*>	mMeshInstancesPool;
		CMaterial* mpMaterial = nullptr;
		CMesh*		  mpMesh = nullptr;
		CShaderInstance*	mpOceanSimulatorShader = nullptr;
		CShaderInstance* mpQuadVS = nullptr;
		CShaderInstance* mpUpdateDisplacementPS = nullptr;
		CShaderInstance* mpGenGradientFoldingPS = nullptr;

		CSurfaceView* mpDisplacement = nullptr;
		CSurfaceView* mpGradientMap = nullptr;

		CSurfaceView* m_pUAV_H0 = nullptr;
		CSurfaceView* m_pUAV_Ht = nullptr;
		CSurfaceView* m_pUAV_Omega = nullptr;
		CSurfaceView* m_pUAV_Dxyz = nullptr;
		// Samplers
		ID3D11SamplerState* mpCubeSampler = nullptr;

		// State blocks
		ID3D11BlendState* mpBStateTransparent = nullptr;


		RenderState* mpOceanRS  = nullptr;
		//RenderState* mpBStateTransparent = nullptr;

		// Samplers
		//ID3D11SamplerState* mpPointSamplerState = nullptr;

		//CTexture mDisplacementMap;
		//CTexture mGradientMap;

		CTexture mFresnel;
		FFT_Plan m_fft_plan;

		ID3D11Buffer* mpQuadVB = nullptr;

		// Quad-tree LOD, 0 to 9 (1x1 ~ 512x512) 
		int mLods = 0;
		// Pattern lookup array. Filled at init time.
		QuadRenderParam mMeshPatterns[9][3][3][3][3];

		// Rendering list
		std::vector<QuadNode> mRenderNodes;
#ifdef CS_DEBUG_BUFFER
		ID3D11Buffer* mpDebugBuffer = nullptr;
#endif
	};

}
