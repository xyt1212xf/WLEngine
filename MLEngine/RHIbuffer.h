#pragma once
#include "Refcount.h"

namespace ML
{
	class CRHIBuffer : public CRefcount
	{
	public:
		uint32 GetSize() const { return Size; }
		uint32 GetStride() const { return Stride; }

	protected:
		uint32 Size = 0;
		uint32 Stride = 0;
	};
}