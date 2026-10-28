#pragma once
#include "Object.h"
#include "Common.h"
#include <d3d12.h>

namespace ML
{
	// UE: UMaterial::BlendMode (EMaterialBlendMode).
	// Drives the PSO blend state + depth-write mask, and (later) which render
	// list the primitive lands in (opaque vs translucent). A bool could not
	// express BLEND_Masked: masked reads alpha like a transparent material but
	// still writes depth and sorts with the opaque pass, so it is NOT "translucent".
	enum class EBlendMode : uint8
	{
		BLEND_Opaque = 0,   // no blending, writes depth
		BLEND_Masked,       // alpha-test cutout, no blending, writes depth
		BLEND_Translucent,  // alpha blending, no depth write
		BLEND_Additive,     // src + dst, no depth write
		BLEND_Modulate,     // src * dst, no depth write
	};

	class CShader;
	class CGlobalShader;
	class CPipelineState;
	class CStaticVertexFactory;
	class CTexture2D;

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
	//   3. Materials are never created/destroyed on the _Flush path - with one
	//      deliberate exception: the PSO for a given (Material, VertexFactory)
	//      pair is built the first time a draw asks for it (see
	//      GetPipelineState). What it does mean is that no CMaterial instance
	//      itself is ever new'd/deleted during rendering.
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
		// Set before the first GetPipelineState() call: the PSO is built from whatever
		// shader names are current at that moment.
		void SetShaders(const std::string& InVSName, const std::string& InPSName);

		// Sets a fallback InputLayout the material owns. Used when a PSO is built without
		// a VertexFactory (or with a VF that has no layout yet) - the typical case is the
		// engine's Default Material at boot time, before any mesh has been loaded.
		// P0: caller passes a hand-written element list. P1: can be derived from shader reflection.
		// Calling this multiple times replaces the previous layout.
		void SetDefaultInputLayout(std::initializer_list<D3D12_INPUT_ELEMENT_DESC> Elements);
		void SetDefaultInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& Elements);

		// Read-only accessors.
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetDefaultInputLayout() const { return mDefaultInputLayout; }
		bool HasDefaultInputLayout() const { return !mDefaultInputLayout.empty(); }

		// Declares whether this material's shaders sample the base-color texture
		// (root parameter 1/2 of the textured root signature).
		//
		// Why it has to be stated instead of derived: nothing here can look at a
		// compiled shader and see whether it reads t0 (no PSO reflection yet),
		// and binding a descriptor table into a root signature that does not have
		// one is an API error - while NOT binding into one that does have it makes
		// the shader read an undefined descriptor. So the material says which it is,
		// and the renderer can then turn "texture missing" into "skip the draw"
		// instead of undefined pixels.
		// UE derives this from the compiled material's uniform expressions.
		void EnableBaseColorTexture(bool bInUses) { bUsesBaseColorTexture = bInUses; }
		bool UsesBaseColorTexture() const { return bUsesBaseColorTexture; }

		// Base-color texture parameter.
		// UE: UTexture* parameter of a UMaterialInterface; the renderer reads it
		// off the FMaterialRenderProxy. P0: one slot, no per-instance override
		// (no render proxy yet - see the MeshBatch comment).
		void SetBaseColorTexture(CTexture2D* InTexture);
		CTexture2D* GetBaseColorTexture() const { return BaseColorTexture; }

		// True when there is something to bind: a texture pointer alone is not
		// enough, the asset must have finished its GPU upload.
		bool HasBaseColorTexture() const;

		// Returns the PSO matching this VertexFactory, building it on first use.
		//
		// UE: the PSO is the product of (Material, VertexFactory, RenderTarget,
		// pass), which is why UE holds a cache keyed by that tuple instead of one
		// PSO per material. Keying by INPUT LAYOUT here gets most of that for
		// free: two meshes with identical layouts share one PSO, and a mesh whose
		// layout differs (e.g. no color stream) gets its own.
		//
		// Layout resolution order inside:
		//   1. InVertexFactory's layout (if VF exists and HasInputLayout())
		//   2. Material's own mDefaultInputLayout (fallback: boot-time DefaultMaterial)
		//   3. nullptr -> return nullptr
		// Thread-safe: called by the renderer for every draw, first call builds.
			// bWireframe folds into the cache key so a wireframe PSO is never confused
		// with its solid twin (same layout). UE bakes the fill mode into the full
		// PSO hash, not the layout hash.
		CPipelineState* GetPipelineState(const CStaticVertexFactory* InVertexFactory, bool bWireframe = false);

		// Drops every cached PSO (releases the GPU objects behind them).
		void ReleasePipelineStates();

		// Read-only access for the render thread.
		CShader* GetVertexShader() const { return VertexShader; }
		CShader* GetPixelShader()  const { return PixelShader; }

		// Blend mode (UE: UMaterial::BlendMode). Sets how the PSO blends and
		// which render list the owning primitive goes to. Set before the first
		// GetPipelineState() so the built PSO carries the right blend state.
		void SetBlendMode(EBlendMode InMode) { BlendMode = InMode; }
		EBlendMode GetBlendMode() const { return BlendMode; }

		// UE: UMaterial::IsTransparent(). Masked is NOT translucent: it sorts and
		// writes depth with the opaque pass, only its pixels are alpha-tested.
		bool IsTranslucent() const
		{
			return BlendMode == EBlendMode::BLEND_Translucent
				|| BlendMode == EBlendMode::BLEND_Additive
				|| BlendMode == EBlendMode::BLEND_Modulate;
		}

		// Asset-level load hook. Not overriding a base-class virtual any more:
	// CObject has no initialise() (UE: UObject::PostLoad lives on the asset
	// type that needs it, not on UObject).
	virtual bool initialise();

	private:
		std::string   VSName;
		std::string   PSName;
		CShader*      VertexShader = nullptr;   // lifetime not owned (managed by CGlobalShaderMap)
		CShader*      PixelShader  = nullptr;

		// Base-color texture. Not owned: textures belong to CTextureMgr (shared
		// assets), materials only reference them.
		CTexture2D*   BaseColorTexture = nullptr;
		bool          bUsesBaseColorTexture = false;

		// Fallback layout used when no VertexFactory is supplied (e.g. engine init time).
		// Does not own any GPU resources - consumed once during PSO creation to build it.
		std::vector<D3D12_INPUT_ELEMENT_DESC> mDefaultInputLayout;

		// Blend mode: opaque by default, matching UE's BLEND_Opaque material default.
		EBlendMode BlendMode = EBlendMode::BLEND_Opaque;

		// Pipeline states owned by this material, one per distinct input layout.
		// See GetPipelineState() for why the key is the layout and not the VF.
		std::unordered_map<uint64, CPipelineState*>	mPSOCache;
		std::mutex									mPSOMutex;

		CPipelineState* CreatePipelineState(const CStaticVertexFactory* InVertexFactory, bool bWireframe);
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* SelectInputLayout(const CStaticVertexFactory* InVertexFactory) const;
	};
}
