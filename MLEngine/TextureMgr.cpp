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
		std::lock_guard<std::mutex> Lock(Mutex);

		auto It = Textures.find(FilePath);
		if (It != Textures.end())
		{
			return It->second.get();
		}

		std::unique_ptr<CTexture2D> Texture = std::make_unique<CTexture2D>();
		if (!Texture->LoadSourceFromFile(FilePath) || !Texture->InitRHI())
		{
			return nullptr;
		}

		CTexture2D* Result = Texture.get();
		Textures.emplace(FilePath, std::move(Texture));
		return Result;
	}

	CTexture2D* CTextureMgr::Find(const std::string& FilePath) 
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		auto It = Textures.find(FilePath);
		return (It != Textures.end()) ? It->second.get() : nullptr;
	}
}
