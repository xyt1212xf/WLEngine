#pragma once
#include "Plug.h"
namespace ML
{
	class CRHIDevice;
	class CRenderer;
	class MLENGINE_API CGraphicPlug : public CPlug
	{
	public:
		CGraphicPlug();
		virtual ~CGraphicPlug();

		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;
		virtual void Process(float DeltaSeconds) const override final;
		virtual void Start() override final;
		CRHIDevice* GetDevice();

	private:
		CRHIDevice*	mpDeviceBase = nullptr;
		CRenderer*		mpRenderer = nullptr;
	};
}
