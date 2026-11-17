#include "SkyLightSceneProxy.h"

namespace ML
{
	CSkyLightSceneProxy::CSkyLightSceneProxy(const FInitParams& InParams)
	{
		Id = InParams.Id;
		LightColor = InParams.LightColor;
		for (int32 i = 0; i < 9; ++i)
		{
			IrradianceSH[i] = InParams.IrradianceSH[i];
		}
		//Cubemap = InParams.Cubemap;
		AverageBrightness = InParams.AverageBrightness;
		BlendFraction = InParams.BlendFraction;
		bRealTimeCaptureEnabled = InParams.bRealTimeCaptureEnabled;
		CapturePosition = InParams.CapturePosition;
		bVisible = InParams.bVisible;
	}
}
