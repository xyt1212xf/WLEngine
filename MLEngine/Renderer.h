#pragma once
#include "Common.h"
#include "MeshBatch.h"

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

	class MLENGINE_API CRenderer
	{
		friend class CGraphicPlug;
	public:
		CRenderer(CGraphicPlug* plug);
		virtual ~CRenderer();
		
	private:
		void _Draw();
		bool _Begin();
		void _Flush();
		void _End();

	private:
		CGraphicPlug*	mpGraphicPlug = nullptr;
		// PSOs are no longer owned by the renderer: each CMaterial builds its own in InitRHI; renderer is read-only.

		// 每帧可见批次。做成成员而不是局部变量：
		//   - clear() 保留 capacity，跨帧复用同一块缓冲区，避免每帧重新扩容拷贝
		//     （UE 同理：FSceneRenderer 持有数组，每帧 Reset 而非重建）
		//   - 不能用函数内 static：那是跨实例/跨线程共享的可变状态
		// 渲染线程独占 CRenderer，故无需加锁。
		std::vector<FMeshBatch> mVisibleBatches;
	};
}
