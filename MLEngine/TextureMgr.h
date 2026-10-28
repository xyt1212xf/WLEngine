#pragma once
#include "Mgr.h"
#include "Common.h"
#include "Texture2D.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace ML
{
	class CTexture2D;

	//====================================================================
	// Engine texture subsystem (instance class, owned by CEngine, inherits CMgr).
	//
	// UE counterpart: the texture half of the asset registry plus the engine's
	// built-in textures (GEngine->DefaultTexture / GWhiteTexture).
	//
	// Difference from CMaterialMgr: this one OWNS what it loads. A material has
	// natural owners already (the CStaticMesh it was made for, or the level that
	// authored it); a texture does not - it is shared by every material that
	// references the same file, which is exactly what a registry is for.
	//====================================================================
	class MLENGINE_API CTextureMgr : public CMgr
	{
	public:
		// Declared out of line: unique_ptr<CTexture2D> needs the complete type
		// at destruction, and only this TU has it.
		virtual ~CTextureMgr();

		virtual const char* GetName() const override { return "TextureMgr"; }

		// P0: nothing to do yet - textures are created per request, not at boot.
		virtual bool Init() override;
		virtual void Shutdown() override;

		// Loads FilePath (relative to the working directory, like every other
		// asset path in the engine) and returns the cached asset on later calls.
		// A decode or upload failure is not cached: returning nullptr lets the
		// caller retry after the GPU is up, while leaving the caller free to
		// ignore the failure (the renderer just skips missing textures).
		CTexture2D* Load(const std::string& FilePath);

		CTexture2D* Find(const std::string& FilePath);

	private:
		// Levels are parsed on worker threads, so two loads may race.
		// mutable: Find() is logically read-only (and therefore const) yet must
		// still take the lock; Load() also locks it on the write path.
		std::mutex Mutex;

		// Keyed by source path - UE would key by asset name; without the asset
		// registry the path IS the identity of a P0 texture.
		std::unordered_map<std::string, std::unique_ptr<CTexture2D>> Textures;
	};
}
