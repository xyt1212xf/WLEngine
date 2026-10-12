#include "Renderer.h"
#include "MLEngine.h"
#include "TimerClock.h"
#include "GraphicPlug.h"
#include "Device.h"
#include "World.h"
#include "MeshBatch.h"
#include "ViewMatrices.h"
#include "MathLib.h"
#include "GlobalShader.h"
#include "SolidColorShader.h"
#include "PipelineState.h"
#include "VertexStreamFactory.h"
#include "RHIbuffer.h"
#include "StaticMeshResources.h"

namespace ML
{
	CRenderer::CRenderer(CGraphicPlug* plug)
	: mpGraphicPlug(plug)
	{

	}

	CRenderer::~CRenderer()
	{
		SafeDelete(mpSolidColorPSO);
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
			double costTime = time.getTimerSecond();
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
		Math::buildMatrixPerspectiveFovLH(&Proj, Math::angleToRadian(60.0f), Aspect, 0.1f, 100.0f);
		FViewMatrices ViewMatrices(View, Proj);

		TArray<FMeshBatch> Batches;
		pWorld->CollectVisibleMeshes(ViewMatrices, Batches);
		if (Batches.IsEmpty() || nullptr == Batches[0].VertexFactory)
		{
			return;
		}

		CSolidColorVS* pVS = CGlobalShaderMap::GetShader<CSolidColorVS>();
		CSolidColorPS* pPS = CGlobalShaderMap::GetShader<CSolidColorPS>();
		if (nullptr == pVS || nullptr == pPS || !pVS->IsValid() || !pPS->IsValid())
		{
			return;
		}

		// Create the solid-color PSO lazily on first use. Input layout comes
		// from the vertex factory of the first batch.
		if (nullptr == mpSolidColorPSO)
		{
			TArray<D3D12_INPUT_ELEMENT_DESC> InputLayout =
				Batches[0].VertexFactory->BuildInputLayout({ "POSITION", "COLOR" });
			mpSolidColorPSO = new CPipelineState();
			if (!mpSolidColorPSO->Create(pVS, pPS, InputLayout, DXGI_FORMAT_D32_FLOAT))
			{
				SafeDelete(mpSolidColorPSO);
				return;
			}
		}

		// Bind pipeline state once per frame.
		pCmd->SetGraphicsRootSignature(mpSolidColorPSO->GetRootSignature());
		pCmd->SetPipelineState(mpSolidColorPSO->GetPipelineState());
		pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (int32 i = 0; i < Batches.Num(); ++i)
		{
			const FMeshBatch& Batch = Batches[i];
			if (nullptr == Batch.LOD || nullptr == Batch.LOD->IndexBuffer || nullptr == Batch.VertexFactory)
			{
				continue;
			}

			const TArray<D3D12_VERTEX_BUFFER_VIEW>& VBViews = Batch.VertexFactory->mCachedVerticesViews;
			if (VBViews.IsEmpty())
			{
				continue;
			}

			// Row-vector convention: clip = pos * (World * View * Proj).
			FMatrix44 MVP = Batch.WorldMatrix * ViewMatrices.ViewMatrix * ViewMatrices.ProjMatrix;

			pCmd->IASetVertexBuffers(0, VBViews.Num(), VBViews.GetData());
			const D3D12_INDEX_BUFFER_VIEW& IndexBufferView = Batch.LOD->IndexBuffer->GetIndexBufferView(); 
			pCmd->IASetIndexBuffer(&IndexBufferView);
			// Root constant slot 0: 16 floats = the 4x4 MVP matrix.
			pCmd->SetGraphicsRoot32BitConstants(0, 16, &MVP, 0);
			pCmd->DrawIndexedInstanced(Batch.Element.NumIndices, 1,
				Batch.Element.FirstIndex, Batch.Element.BaseVertex, 0);
		}
	}

	void CRenderer::_End()
	{
		mpGraphicPlug->GetDevice()->EndDraw();	
	}

}
