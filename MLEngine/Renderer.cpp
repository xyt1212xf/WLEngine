#include "Renderer.h"
#include "MLEngine.h"
#include "TimerClock.h"
#include "GraphicPlug.h"
#include "Device.h"
#include "World.h"
#include "MeshBatch.h"
#include "ViewMatrices.h"
#include "MathLib.h"
#include "Material.h"
#include "MaterialMgr.h"
#include "PipelineState.h"
#include "VertexStreamFactory.h"
#include "RHIbuffer.h"
#include "StaticMeshResources.h"
#include "Texture2D.h"
#include "RHItexture.h"

namespace ML
{
	// 可见批次的初始预留容量，省掉最初几次 2 倍扩容。
	// 只有本文件的构造函数用到，故放在 cpp —— 头文件只描述类接口，
	// 不暴露这种实现细节（改它也只需重编这一个 TU）。
	// constexpr 在命名空间作用域隐含 const，具备内部链接，不会跨 TU 冲突。
	constexpr uint32 InitialBatchReserve = 256;

	CRenderer::CRenderer(CGraphicPlug* plug)
	: mpGraphicPlug(plug)
	{
		// 一次性预留，之后每帧只 clear() 不再重新分配。
		mVisibleBatches.reserve(InitialBatchReserve);
	}

	CRenderer::~CRenderer()
	{
	}

	void CRenderer::_Draw()
	{
		static CTimerClock time;
		while (GEngine->IsRun())
		{
			time.begin();
			if (_Begin())
			{
				_Flush();
			}
			_End();
			//double costTime = time.getTimerSecond();
		}
	}

	bool CRenderer::_Begin()
	{
		mpGraphicPlug->GetDevice()->BeginDraw();	
		return true;
	}

	void CRenderer::_Flush()
	{
		// Skip until the world finished loading (Parsing -> ... -> Ready).
		CWorld* pWorld = GEngine->GetWorld();
		if (nullptr == pWorld || !pWorld->IsReady())
		{
			return;
		}

		CRHIDevice* pRHIDevice = mpGraphicPlug->GetDevice();
		ID3D12GraphicsCommandList* pCmd = pRHIDevice->GetCommandList();

		// Hardcoded camera for the first bring-up: eye at origin looking
		// down +Z (LH), 60 deg fov. A proper CCameraComponent comes later.
		const SWindowConfig* pCfg = GEngine->GetPlatform().getWindowConfigPtr();
		const float Aspect = (float)pCfg->nWidth / (float)pCfg->nHeight;
		FMatrix44 View, Proj;
		const Vec3F Eye(0.0f, 0.0f, 0.0f);
		const Vec3F LookAt(0.0f, 0.0f, 1.0f);
		const Vec3F Up(0.0f, 1.0f, 0.0f);
		Math::buildMatrixLookAtLH(&View, &Eye, &LookAt, &Up);
		Math::buildMatrixPerspectiveFovLH(&Proj, Math::angleToRadian(60.0f), Aspect, 0.1f, 5000.0f);
		FViewMatrices ViewMatrices(View, Proj);

		// Draw mode: a view-family-level override (UE: FSceneViewFamily::EngineShowFlags).
		// Folded into the material's PSO cache key so wireframe and solid get distinct PSOs.
		const bool bWireframe = mViewFamily.IsWireframe();

		// 3) Collect all batches to draw (each already carries its Material*).
		//    复用成员缓冲区：clear() 只归零 size，capacity 保留。
		mVisibleBatches.clear();
		pWorld->CollectVisibleMeshes(ViewMatrices, mVisibleBatches);
		if (mVisibleBatches.empty())
		{
			return;
		}

		// 4) Bucket by material: batches sharing a material merge into one PSO bind.
		//    Simplified P0: set the PSO once for the first valid material; later
		//    batches only switch when the Material changes.
		//    UE counterpart: FMeshPassProcessor::BuildMeshDrawCommands.
		CPipelineState* CurrentPSO = nullptr;
		ID3D12RootSignature* CurrentRS = nullptr;

		for (int32 i = 0; i < mVisibleBatches.size(); ++i)
		{
			const FMeshBatch& Batch = mVisibleBatches[i];
			if (nullptr == Batch.LOD
				|| nullptr == Batch.LOD->IndexBuffer
				|| nullptr == Batch.VertexFactory
				|| nullptr == Batch.Material)
			{
				continue;
			}

			// The PSO depends on the mesh's vertex layout, so it is resolved per
			// batch and built on first use.
			CPipelineState* PSO = Batch.Material->GetPipelineState(Batch.VertexFactory, bWireframe);
			if (nullptr == PSO)
			{
				continue;
			}

			// A material whose shaders sample the base-color texture but whose
			// asset is missing is skipped rather than drawn with an undefined
			// descriptor - see CMaterial::EnableBaseColorTexture.
			if (Batch.Material->UsesBaseColorTexture() && !Batch.Material->HasBaseColorTexture())
			{
				continue;
			}

			const std::vector<D3D12_VERTEX_BUFFER_VIEW>& VBViews = Batch.VertexFactory->mCachedVerticesViews;
			if (VBViews.empty())
			{
				continue;
			}

			// Switch PSO / RootSignature (only when the material changes).
			if (PSO != CurrentPSO)
			{
				CurrentPSO = PSO;
				CurrentRS  = PSO->GetRootSignature();
				pCmd->SetGraphicsRootSignature(CurrentRS);
				pCmd->SetPipelineState(PSO->GetPipelineState());
				pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}

			// Compute MVP and bind it to root constant slot 0 (16 floats).
			// Row-vector: clip = pos * (World * View * Proj).
			FMatrix44 MVP = Batch.WorldMatrix * ViewMatrices.ViewMatrix * ViewMatrices.ProjMatrix;
			pCmd->SetGraphicsRoot32BitConstants(0, 16, &MVP, 0);

			// Per-material textures (root slots 1 = SRV, 2 = sampler).
			// UE: FMeshPassProcessor writes these into the FMeshDrawCommand's
			// shader bindings; here they are pushed per draw instead. Materials
			// that do not sample a texture leave their root signature alone.
			if (Batch.Material->HasBaseColorTexture())
			{
				ID3D12DescriptorHeap* Heaps[2] =
				{
					pRHIDevice->GetSrvHeap(),
					pRHIDevice->GetSamplerHeap()
				};
				pCmd->SetDescriptorHeaps(2, Heaps);
				pCmd->SetGraphicsRootDescriptorTable(1, Batch.Material->GetBaseColorTexture()->GetRHITexture()->GetSRVGPUHandle());
				pCmd->SetGraphicsRootDescriptorTable(2, pRHIDevice->GetStaticSamplerGPUHandle());
			}

			// Geometry.
			pCmd->IASetVertexBuffers(0, (UINT)VBViews.size(), VBViews.data());
			const D3D12_INDEX_BUFFER_VIEW& IndexBufferView = Batch.LOD->IndexBuffer->GetIndexBufferView();
			pCmd->IASetIndexBuffer(&IndexBufferView);
			pCmd->DrawIndexedInstanced(Batch.Element.NumIndices, 1,
				Batch.Element.FirstIndex, Batch.Element.BaseVertex, 0);
		}
	}

	void CRenderer::_End()
	{
		mpGraphicPlug->GetDevice()->EndDraw();
	}

}
