#pragma once
#include "Plug.h"
namespace ML
{
	class CDX12RHIDevice;
	class CRenderer;
	class MLENGINE_API CGraphicPlug : public CPlug
	{
	public:
		CGraphicPlug();
		virtual ~CGraphicPlug();

		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;
		virtual void Process(int32 DeltaSeconds) const override final;
		virtual void Start() override final;
		CDX12RHIDevice* GetDevice();

	private:
		CDX12RHIDevice*	mpDeviceBase = nullptr;
		CRenderer*		mpRenderer = nullptr;
	};
}
