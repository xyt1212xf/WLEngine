#include "TextureMgr.h"
#include "Texture2D.h"

namespace ML
{
	CTextureMgr::~CTextureMgr()
	{
		Shutdown();
	}

	bool CTextureMgr::Init()
	{
		// No preset textures yet: the only textures that exist today are the ones
		// a level asks for. Keeping Init for symmetry with the other subsystems.
		return true;
	}

	void CTextureMgr::Shutdown()
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		Textures.clear();
	}

	CTexture2D* CTextureMgr::Load(const std::string& FilePath)
	{
		// Fast path: already cached? Return it without touching the file or the GPU.
		// The key idea is to keep the expensive decode + upload OUT of the lock, so a
		// parallel Load of a *different* asset (or a second Load of this one) never
		// blocks on another thread's file I/O / GPU upload.
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			auto It = Textures.find(FilePath);
			if (It != Textures.end())
			{
				return It->second.get();
			}
		}

		// Slow path: build the texture with NO lock held. LoadSourceFromFile (WIC
		// decode) and InitRHI (GPU upload + SRV alloc) are the heavy part.
		std::unique_ptr<CTexture2D> Texture = std::make_unique<CTexture2D>();
		if (!Texture->LoadSourceFromFile(FilePath) || !Texture->InitRHI())
		{
			return nullptr;
		}

		// Re-lock only to publish. emplace returns <iterator, bool>: if the key was
		// absent our texture lands (bool == true); if a peer thread already inserted
		// the same path first, emplace refuses to overwrite and destroys our copy
		// (its ~CTexture2D frees the RHI + SRV slot), while the iterator still points
		// at the surviving winner. Either way iterator->second is the single live
		// instance, so we return that - no second find, and never a dangling pointer.
		std::lock_guard<std::mutex> Lock(Mutex);
		return Textures.emplace(FilePath, std::move(Texture)).first->second.get();
	}

	CTexture2D* CTextureMgr::Find(const std::string& FilePath) 
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		auto It = Textures.find(FilePath);
		return (It != Textures.end()) ? It->second.get() : nullptr;
	}
}
