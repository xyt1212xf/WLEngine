#pragma once
#include "DX12RHItexture.h"

namespace ML
{
	// Same "abstract alias" convention as RHIbuffer.h: the API-neutral name the
	// rest of the engine talks to, resolved to the DX12 implementation until the
	// graphics-API switch needs to be able to swap it out.
	class CRHITexture2D : public CDX12RHITexture2D
	{
	public:
		CRHITexture2D() = default;
		virtual ~CRHITexture2D() = default;
	};
}
