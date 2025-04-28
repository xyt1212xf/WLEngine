#pragma once
#include "Device.h"
#include "SurfaceView.h"

namespace WL
{
	struct 	DX11_FFT_Plan
	{
		// D3D11 objects
		ID3D11DeviceContext* pContext = nullptr;
		//ID3D11ComputeShader* pRadix008A_CS = nullptr;
		//ID3D11ComputeShader* pRadix008A_CS2 = nullptr;
		CShaderInstance* pRadix008A_CS = nullptr;
		CShaderInstance* pRadix008A_CS2 = nullptr;
		// For 512x512 config, we need 6 constant buffers
		CSurfaceView mRadix008A_CB[6];

		// Temporary buffers
		CSurfaceView* pTmpUAV;

		~DX11_FFT_Plan()
		{
			pContext = nullptr;
			WL_DELETE(pTmpUAV, SurfaceView);
			WL_DELETE(pRadix008A_CS, SurfaceView);
			WL_DELETE(pRadix008A_CS2, SurfaceView);
		}
	};
}
