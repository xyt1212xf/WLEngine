#include "GraphicPlug.h"
#include "MLEngine.h"
#include "Device.h"
#include "Renderer.h"
#include "TimerClock.h"
#include "RHIbuffer.h"
#include "GlobalShader.h"
#include "MaterialMgr.h"
#include "Material.h"
#include "SolidColorShader.h"

namespace ML
{

	CGraphicPlug::CGraphicPlug()
	{
#ifdef _DEBUG
		std::cout<<this;
#endif
	}

	CGraphicPlug::~CGraphicPlug()
	{

	}


	bool CGraphicPlug::Initialise()
	{
		if (nullptr == mpDeviceBase)
		{
			mpDeviceBase = new CRHIDevice;
			if (mpDeviceBase->initDevice(GEngine->GetPlatform().getMainWnd()))
			{
				// Compile all registered global shaders once the device is ready.
				CGlobalShaderMap::InitGlobalShaders();
				mpRenderer = new CRenderer(this);

				mpDefaultMaterial = new CMaterial();
				mpDefaultMaterial->SetShaders(CSolidColorVS::StaticShaderName(), CSolidColorPS::StaticShaderName());
				// Default Material's fallback layout (POSITION slot 0, COLOR slot 1).
				// Matches the attribute set expected by CSolidColorVS / CSolidColorPS.
				// P0: hand-written. P1: derive from shader reflection.
				mpDefaultMaterial->SetDefaultInputLayout({
					D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					D3D12_INPUT_ELEMENT_DESC{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				});
				// InitRHI's first argument is the per-mesh VF; pass nullptr here so the material
				// uses its own mDefaultInputLayout (boot-time path - no mesh loaded yet).
				if (!mpDefaultMaterial->InitRHI(nullptr, DXGI_FORMAT_D32_FLOAT))
				{
					SafeDelete(mpDefaultMaterial);
					return false;
				}

				if( CMaterialMgr* MatMgr = GEngine->GetMgr<CMaterialMgr>())
				{
					MatMgr->Register("Default", mpDefaultMaterial);
					MatMgr->SetDefaultMaterial(mpDefaultMaterial);
				}
				else
				{
					return false;
				}
			}
		}	
		return true;
	}

	bool CGraphicPlug::UnInitialise()
	{
		CGlobalShaderMap::DestroyGlobalShaders();
		SafeDelete(mpRenderer);
		SafeDelete(mpDeviceBase);
		SafeDelete(mpDefaultMaterial);
		SafeDelete(mpDeviceBase);
		return true;
	}

	void CGraphicPlug::Process(float DeltaSeconds) const
	{

	}

	void CGraphicPlug::Start()
	{
		GEngine->threadDetach([&]()
			{
				CThreadPool::setThreadName("RenderThread", GetCurrentThreadId());
				mpRenderer->_Draw();
			});
	}

	CRHIDevice* CGraphicPlug::GetDevice()
	{
		return mpDeviceBase;
	}

}
