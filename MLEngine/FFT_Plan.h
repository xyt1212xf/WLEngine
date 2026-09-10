#pragma once

#ifdef D3D11
#include "DX11FFT_Plan.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Common constants
////////////////////////////////////////////////////////////////////////////////

#define FFT_DIMENSIONS 3U
#define FFT_PLAN_SIZE_LIMIT (1U << 27)

#define FFT_FORWARD -1
#define FFT_INVERSE 1
namespace WL
{
#ifdef D3D11
	struct SFFT_Plan : public SDX11_FFT_Plan
#endif
	{
		// More than one array can be transformed at same time
		UINT slices;
	};
}
