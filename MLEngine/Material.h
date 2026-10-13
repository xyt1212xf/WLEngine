#pragma once
#include "Resource.h"
#include "Common.h"
#include <d3d12.h>
#include "Array.h"

namespace ML
{
	class CShader;
	class CGlobalShader;
	class CPipelineState;
	class CStaticVertexFactory;

	// Engine material asset (CResource subclass).
	// UE counterpart: UMaterial + FMaterial (merged in P0: no per-draw parameter
	// variants, no separate RenderProxy).
	//
	// Design discipline (same as UE):
	//   1. Material knows nothing about InputLayout - the VertexFactory provides it.
	//      A PSO is the product of the triple (Material, VertexFactory, RenderTarget).
	//   2. Material knows nothing about RenderTarget / DepthStencil - depth format
	//      is the renderer's / render pass's business. In P0 InitRHI takes a
	//      DepthFormat parameter, but only as a PSO creation argument; the material
	//      does not store it.
	//   3. Materials are never created/destroyed on the _Flush path - that only
	//      happens during engine Init / Shutdown.
	//   4. Material does not store shader name strings (in P2, once upgraded to
	//      FMaterialShaderMap, this becomes TMap<FShaderType*, TShaderRef>);
	//      P0 keeps VSName/PSName to interface with IMPLEMENT_GLOBAL_SHADER
	//      registration.
	class MLENGINE_API CMaterial : public CResource
	{
	public:
		CMaterial();
		virtual ~CMaterial();

		// Sets the VS/PS this material uses (looked up by CGlobalShader name in GlobalShaderMap).
		// Call order: SetShaders(...) -> InitRHI(VF, Depth) -> then it is usable.
		void SetShaders(const std::string& InVSName, const std::string& InPSName);

		// Creates the PSO. Layout comes from the VertexFactory; material holds no semantic concept.
		// Must be called after the RHI device is ready and InVertexFactory has run BuildInputLayout.
		bool InitRHI(const CStaticVertexFactory* InVertexFactory, DXGI_FORMAT DepthFormat);

		// Releases RHI resources.
		void ReleaseRHI();

		// Read-only access for the render thread.
		CPipelineState* GetPipelineState() const { return PipelineState; }
		CShader* GetVertexShader() const { return VertexShader; }
		CShader* GetPixelShader()  const { return PixelShader; }

		virtual bool initialise() override;

	private:
		std::string   VSName;
		std::string   PSName;
		CShader*      VertexShader = nullptr;   // lifetime not owned (managed by CGlobalShaderMap)
		CShader*      PixelShader  = nullptr;
		CPipelineState* PipelineState = nullptr; // owned
	};
}
