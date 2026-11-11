#include "TextureMgr.h"
#include "Texture2D.h"
#include <cmath>

namespace ML
{
	namespace
	{
		// Bilinear RGBA8 resample. Albedo layers legitimately differ in resolution
		// (UE allows it too), but a Texture2DArray is uniform in W x H, so every
		// slice is brought to the first layer's size instead of failing the load.
		void ResampleRGBA(const std::vector<uint8>& Src, uint32 SrcW, uint32 SrcH,
			std::vector<uint8>& Dst, uint32 DstW, uint32 DstH)
		{
			Dst.clear();
			if (Src.size() != (size_t)SrcW * SrcH * 4 || 0 == SrcW || 0 == SrcH || 0 == DstW || 0 == DstH)
			{
				return;
			}
			Dst.resize((size_t)DstW * DstH * 4);

			const float ScaleX = (float)SrcW / (float)DstW;
			const float ScaleY = (float)SrcH / (float)DstH;

			for (uint32 y = 0; y < DstH; ++y)
			{
				const float fy = ((float)y + 0.5f) * ScaleY - 0.5f;
				int32 y0 = (int32)floorf(fy);
				const float ty = fy - (float)y0;
				int32 y1 = y0 + 1;
				y0 = (y0 < 0) ? 0 : ((y0 >= (int32)SrcH) ? (int32)SrcH - 1 : y0);
				y1 = (y1 < 0) ? 0 : ((y1 >= (int32)SrcH) ? (int32)SrcH - 1 : y1);

				for (uint32 x = 0; x < DstW; ++x)
				{
					const float fx = ((float)x + 0.5f) * ScaleX - 0.5f;
					int32 x0 = (int32)floorf(fx);
					const float tx = fx - (float)x0;
					int32 x1 = x0 + 1;
					x0 = (x0 < 0) ? 0 : ((x0 >= (int32)SrcW) ? (int32)SrcW - 1 : x0);
					x1 = (x1 < 0) ? 0 : ((x1 >= (int32)SrcW) ? (int32)SrcW - 1 : x1);

					const uint8* P00 = &Src[((size_t)y0 * SrcW + x0) * 4];
					const uint8* P10 = &Src[((size_t)y0 * SrcW + x1) * 4];
					const uint8* P01 = &Src[((size_t)y1 * SrcW + x0) * 4];
					const uint8* P11 = &Src[((size_t)y1 * SrcW + x1) * 4];
					// NB: do not name this "Out" - sal.h defines Out as a SAL
					// annotation macro on MSVC and it rewrites the declaration.
					uint8* DstPixel = &Dst[((size_t)y * DstW + x) * 4];

					for (int c = 0; c < 4; ++c)
					{
						const float Top    = (float)P00[c] + ((float)P10[c] - (float)P00[c]) * tx;
						const float Bottom = (float)P01[c] + ((float)P11[c] - (float)P01[c]) * tx;
						const float Value  = Top + (Bottom - Top) * ty;
						DstPixel[c] = (uint8)((Value < 0.0f) ? 0.0f : ((Value > 255.0f) ? 255.0f : Value));
					}
				}
			}
		}
	}

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

	CTexture2D* CTextureMgr::LoadLayerArray(const std::string& CacheKey, const std::vector<std::string>& Paths)
	{
		if (Paths.empty() || Paths.size() > 4)
		{
			OutputDebugStringA("CTextureMgr::LoadLayerArray: needs 1..4 layer paths\n");
			return nullptr;
		}
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			auto It = Textures.find(CacheKey);
			if (It != Textures.end())
			{
				return It->second.get();
			}
		}
		std::vector<std::vector<uint8>> Slices;
		Slices.reserve(Paths.size());
		uint32 Width = 0;
		uint32 Height = 0;
		for (const std::string& Path : Paths)
		{
			std::vector<uint8> Pixels;
			uint32 W = 0, H = 0;
			if (!CTexture2D::DecodeFileToRGBA(Path, Pixels, W, H))
			{
				return nullptr;
			}
			if (0 == Width)
			{
				Width = W;
				Height = H;
				Slices.push_back(std::move(Pixels));
			}
			else if (W != Width || H != Height)
			{
				// Texture2DArray slices share one size, but albedo layers rarely
				// match: resample to the first layer instead of failing the whole
				// material (which would silently hide the mesh - the renderer
				// skips draws whose declared texture is missing).
				std::vector<uint8> Resampled;
				ResampleRGBA(Pixels, W, H, Resampled, Width, Height);
				if (Resampled.empty())
				{
					OutputDebugStringA(("CTextureMgr::LoadLayerArray: failed to resample \"" + Path + "\"\n").c_str());
					return nullptr;
				}
				OutputDebugStringA(("CTextureMgr::LoadLayerArray: resampled \"" + Path + "\" "
					+ std::to_string(W) + "x" + std::to_string(H) + " -> "
					+ std::to_string(Width) + "x" + std::to_string(Height) + "\n").c_str());
				Slices.push_back(std::move(Resampled));
			}
			else
			{
				Slices.push_back(std::move(Pixels));
			}
		}
		std::unique_ptr<CTexture2D> Texture = std::make_unique<CTexture2D>();
		Texture->SetName(CacheKey);
		Texture->SetLayerSlices(std::move(Slices), Width, Height);
		if (!Texture->InitRHI())
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> Lock(Mutex);
		return Textures.emplace(CacheKey, std::move(Texture)).first->second.get();

	}

}
