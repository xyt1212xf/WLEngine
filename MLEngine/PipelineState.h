#pragma once
#include "DX12PipelineState.h"
namespace ML
{
	class CPipelineState : public CDX12PipelineState
	{
	public:
		CPipelineState() = default;
		virtual ~CPipelineState() = default;
	};
}
