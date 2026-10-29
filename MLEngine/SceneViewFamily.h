#pragma once
#include "NumericLimits.h"

namespace ML
{
	// UE: EViewModeIndex (Editor/UnrealEdTypes.h) — the editor's viewport "draw mode"
	// selector (Lit / Unlit / Wireframe / Lighting Only ...). This is the *source of
	// truth* for how a view is drawn; the renderer never hard-codes a draw mode.
	// Trimmed to what this engine can produce for now; more modes get added as the
	// renderer grows.
	enum class EViewModeIndex : uint8
	{
		VMI_Unknown = 0,
		VMI_Wireframe,    // 线框：rasterizer FillMode = Wireframe
		VMI_Unlit,        // 无光照：只画 base color / 顶点色
		VMI_Lit,          // 真实光照（默认）
		VMI_LightingOnly, // 仅光照贡献（去材质固有色）
	};

	// UE: FEngineShowFlags (Runtime/Engine/Public/ShowFlags.h). In UE this is a large
	// struct of per-feature toggles (Wireframe, Lighting, Materials, BSP, ...). Here
	// we only model the *draw-mode* relevant subset. The renderer reads these flags
	// during pass setup — they are the canonical home of a draw mode, NOT the material
	// and NOT the primitive. Wireframe especially is a view-family-level override, exactly
	// as established earlier when we kept it off CMaterial.
	struct FEngineShowFlags
	{
		bool Wireframe = false; // UE: EngineShowFlags.Wireframe
		bool Lighting  = true;  // UE: EngineShowFlags.Lighting (off => Unlit)

		// UE derives these flags from a chosen view mode via FEditorViewData +
		// ApplyViewModeOverrides. We mirror that with a small helper so a single
		// enum controls the whole family's draw mode.
		static FEngineShowFlags FromViewMode(EViewModeIndex Mode)
		{
			FEngineShowFlags F;
			switch (Mode)
			{
			case EViewModeIndex::VMI_Wireframe:
				F.Wireframe = true;
				F.Lighting  = true;
				break;
			case EViewModeIndex::VMI_Unlit:
				F.Wireframe = false;
				F.Lighting  = false;
				break;
			case EViewModeIndex::VMI_LightingOnly:
				F.Wireframe = false;
				F.Lighting  = true;
				break;
			case EViewModeIndex::VMI_Lit:
			case EViewModeIndex::VMI_Unknown:
			default:
				F.Wireframe = false;
				F.Lighting  = true;
				break;
			}
			return F;
		}
	};

	// UE: FSceneViewFamily (Runtime/Engine/Public/SceneManagement.h). The per-frame
	// description of what and how to render. It owns the scene reference, the render
	// targets, and the linked list of FSceneView. Crucially for draw mode, it owns the
	// shared FEngineShowFlags that ALL its views inherit — so a wireframe toggle is a
	// property of the family (the "view"), never of an individual material.
	//
	// Only the draw-mode-relevant parts are modeled for now (P0 scope): the show flags
	// + a chosen view mode. The Views list / RenderTarget / Scene* are added later as
	// the renderer needs them.
	class FSceneViewFamily
	{
	public:
		FSceneViewFamily() = default;
		explicit FSceneViewFamily(EViewModeIndex InViewMode)
		: ViewMode(InViewMode)
		, ShowFlags(FEngineShowFlags::FromViewMode(InViewMode))
		{
		}
		// Renderer / pass-setup access. Mirrors UE's ViewFamily.EngineShowFlags.* reads.
		FEngineShowFlags GetShowFlags() const { return ShowFlags; }

		// Convenience predicates, the exact names the renderer will call when
		// picking a PSO / override path.
		bool IsWireframe() const { return ShowFlags.Wireframe; }
		bool IsLit()       const { return ShowFlags.Lighting; }

		// Switch draw mode: recomputes the whole family's show flags from the mode,
		// the same way UE applies a viewport view-mode change.
		void SetViewMode(EViewModeIndex InViewMode)
		{
			ViewMode = InViewMode;
			ShowFlags = FEngineShowFlags::FromViewMode(InViewMode);
		}

		EViewModeIndex GetViewMode() const { return ViewMode; }

	private:
		EViewModeIndex   ViewMode  = EViewModeIndex::VMI_Lit;
		FEngineShowFlags ShowFlags = FEngineShowFlags::FromViewMode(EViewModeIndex::VMI_Lit);
	};
}
