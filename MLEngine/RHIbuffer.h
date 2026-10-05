#pragma once
#include "DX12RHIbuffer.h"

namespace ML
{
	class CRHIBuffer : public CDX12RHIBuffer
	{
	public:
		CRHIBuffer() = default;
		virtual ~CRHIBuffer() = default;
		uint32 GetSize() const { return Size; }
		uint32 GetStride() const { return Stride; }
	};
}