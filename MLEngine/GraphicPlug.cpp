#include "GraphicPlug.h"
#include "MLEngine.h"
#include "Device.h"
#include "Renderer.h"
#include "TimerClock.h"
#include "RHIbuffer.h"
#include "GlobalShader.h"
#include "VertexStreamFactory.h"
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
				mpDefaultVertexFactory = new CStaticVertexFactory();
				mpDefaultVertexFactory->NumVertices = 1; // layout derivation needs no real vertex count
				mpDefaultVertexFactory->Schema.Position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				mpDefaultVertexFactory->Schema.Position.Slot = 0;
				mpDefaultVertexFactory->Schema.Color.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				mpDefaultVertexFactory->Schema.Color.Slot = 1;
				// Buffer fields are only needed when rendering via mCachedVerticesViews;
				// for layout-only use they can stay null here.
				// The actual VB binding is provided by the mesh's own VertexFactory at _Flush time.
				std::vector<std::string> Elements = { "POSITION", "COLOR" };
				mpDefaultVertexFactory->BuildInputLayout(Elements);

				mpDefaultMaterial = new CMaterial();
				mpDefaultMaterial->SetShaders(CSolidColorVS::StaticShaderName(), CSolidColorPS::StaticShaderName());
				if (!mpDefaultMaterial->InitRHI(mpDefaultVertexFactory, DXGI_FORMAT_D32_FLOAT))
				{
					SafeDelete(mpDefaultMaterial);
					SafeDelete(mpDefaultVertexFactory);
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
		SafeDelete(mpDefaultVertexFactory);
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
