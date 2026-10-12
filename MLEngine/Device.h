#pragma once
#include "Common.h"
#include "DX12Device.h"

namespace ML
{
	class CRHIDevice : public CDX12RHIDevice
	{
	public:
		CRHIDevice() = default;
		virtual ~CRHIDevice(){};

	};

}