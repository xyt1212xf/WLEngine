#pragma once
#include "Common.h"
#include "MeshBatch.h"
#include "SceneViewFamily.h"
#include "LightShader.h"

namespace ML
{
	enum  RendererType
	{
		Renderer = 0,
		CustomRenderer = 99,
		ShadowRenderer = 100,
		FrontRenderer = 200,
		DeferredRenderer = 300,
		GuiRenderer = 400,
	};

	enum  DrawQualityLevel
	{
		Low,
		Middle,
		Hihg,
	};

	class CPipelineState;
	class CRHIBuffer;

	class MLENGINE_API CRenderer
	{
		friend class CGraphicPlug;
	public:
		CRenderer(CGraphicPlug* plug);
		virtual ~CRenderer();
		bool Initialise();
		// Draw-mode control. UE: the draw mode lives on FSceneViewFamily/ 新增: 光照 4*float4 = 16

		// (a view-family-level override), never on the material or primitive.
		// Switching it recomputes the family's FEngineShowFlags, which the PSO
		// build reads to pick FillMode.
		void SetViewMode(EViewModeIndex InMode) { mViewFamily.SetViewMode(InMode); }
		EViewModeIndex GetViewMode() const { return mViewFamily.GetViewMode(); }

	private:
		void _Draw();
		bool _Begin();
		void _Flush();
		void _End();

	private:
		CGraphicPlug*	mpGraphicPlug = nullptr;
		CRHIBuffer*		mLightConstantsCB = nullptr;
		// UE: FSceneViewFamily — owns the per-frame draw-mode show flags that
		// every PSO built by the materials inherits. Defaults to Lit (solid).
		FSceneViewFamily mViewFamily;
		// PSOs are no longer owned by the renderer: each CMaterial builds its own in InitRHI; renderer is read-only.

		FLightFrameConstants FrameLights = {};

		// 每帧可见批次：在收集阶段就按"不透明 / 透明"分流到两个成员缓冲区
		// （UE: BasePass 列表 vs Translucent pass 列表）。clear() 保留 capacity，
		// 跨帧复用同一块缓冲区，避免每帧重新扩容拷贝。渲染线程独占 CRenderer，无需加锁。
		std::vector<FMeshBatch> mOpaqueBatches;
		std::vector<FMeshBatch> mTranslucentBatches;
	};
}
