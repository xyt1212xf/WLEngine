#pragma once
#include "Object.h"
#include "Common.h"
#include <d3d12.h>

namespace ML
{
	class CShader;
	class CGlobalShader;
	class CPipelineState;
	class CStaticVertexFactory;

	// Engine material asset (CObject subclass).
	// UE counterpart: UMaterial + FMaterial (merged in P0: no per-draw parameter
	// variants, no separate RenderProxy).
	//
	// UE: UMaterialInterface : UObject - materials are NOT UStreamableRenderAsset.
	// They have no LOD/mip payload to drop, so they must not sit on the streaming
	// facet. Naming/registry is handled by CMaterialMgr, not by the base class.
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
	class MLENGINE_API CMaterial : public CObject
	{
	public:
		CMaterial();
		virtual ~CMaterial();

		// Sets the VS/PS this material uses (looked up by CGlobalShader name in GlobalShaderMap).
		// Call order: SetShaders(...) -> [SetDefaultInputLayout(...)] -> InitRHI(VF, Depth) -> then it is usable.
		void SetShaders(const std::string& InVSName, const std::string& InPSName);

		// Sets a fallback InputLayout the material owns. Used when InitRHI is called without
		// a VertexFactory (or with a VF that has no layout yet) - the typical case is the
		// engine's Default Material at boot time, before any mesh has been loaded.
		// P0: caller passes a hand-written element list. P1: can be derived from shader reflection.
		// Calling this multiple times replaces the previous layout.
		void SetDefaultInputLayout(std::initializer_list<D3D12_INPUT_ELEMENT_DESC> Elements);
		void SetDefaultInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& Elements);

		// Read-only accessors.
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetDefaultInputLayout() const { return mDefaultInputLayout; }
		bool HasDefaultInputLayout() const { return !mDefaultInputLayout.empty(); }

		// Creates the PSO. Layout resolution order:
		//   1. InVertexFactory's layout (if VF exists and HasInputLayout())
		//   2. Material's own mDefaultInputLayout (fallback)
		//   3. nullptr -> return false
		// Must be called after the RHI device is ready.
		bool InitPSO(const CStaticVertexFactory* InVertexFactory, DXGI_FORMAT DepthFormat);

		// Releases PSO resources.
		void ReleasePSO();

		// Read-only access for the render thread.
		CPipelineState* GetPipelineState() const { return PipelineState; }
		CShader* GetVertexShader() const { return VertexShader; }
		CShader* GetPixelShader()  const { return PixelShader; }

		// Asset-level load hook. Not overriding a base-class virtual any more:
	// CObject has no initialise() (UE: UObject::PostLoad lives on the asset
	// type that needs it, not on UObject).
	virtual bool initialise();

	private:
		std::string   VSName;
		std::string   PSName;
		CShader*      VertexShader = nullptr;   // lifetime not owned (managed by CGlobalShaderMap)
		CShader*      PixelShader  = nullptr;
		CPipelineState* PipelineState = nullptr; // owned

		// Fallback layout used when no VertexFactory is supplied (e.g. engine init time).
		// Does not own any GPU resources - consumed once during InitRHI to build the PSO.
		std::vector<D3D12_INPUT_ELEMENT_DESC> mDefaultInputLayout;
	};
}
