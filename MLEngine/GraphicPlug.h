#pragma once
#include "Plug.h"
namespace ML
{
	class CRHIDevice;
	class CRenderer;
	class CMaterial;
	class CStaticVertexFactory;
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

		CMaterial* GetDefaultMaterial() const { return mpDefaultMaterial; }
		CStaticVertexFactory* GetDefaultVertexFactory() const { return mpDefaultVertexFactory; }

	private:
		CRHIDevice*	mpDeviceBase = nullptr;
		CRenderer*		mpRenderer = nullptr;
		CMaterial*		mpDefaultMaterial = nullptr;
		CStaticVertexFactory* mpDefaultVertexFactory = nullptr;
	};
}
