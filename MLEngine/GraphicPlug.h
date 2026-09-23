#pragma once
#include "Plug.h"
namespace ML
{
	class CDX12Device;

	class MLENGINE_API CGraphicPlug : public CPlug
	{
	public:
		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;

	private:
		CDX12Device*	mpDeviceBase = nullptr;
	};
}
