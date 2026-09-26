#pragma once
#include "Plug.h"
namespace ML
{
	class CDX12Device;

	class MLENGINE_API CGraphicPlug : public CPlug
	{
	public:
		CGraphicPlug();
		virtual ~CGraphicPlug();

		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;
		virtual void Process(int32 DeltaSeconds) const override final;

	private:
		CDX12Device*	mpDeviceBase = nullptr;
	};
}
