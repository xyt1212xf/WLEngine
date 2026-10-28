#include "Texture2D.h"
#include "RHItexture.h"
#include "MLEngine.h"

// Windows Imaging Component: decodes PNG / JPEG / BMP into RGBA8.
// Chosen over a third-party decoder because it ships with the OS and therefore
// adds no dependency to the ThirdLibrary tree.
#include <windows.h>
#include <objbase.h>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

namespace ML
{
	CTexture2D::CTexture2D()
	{
	}

	CTexture2D::~CTexture2D()
	{
		ReleaseRHI();
	}

	bool CTexture2D::LoadSourceFromFile(const std::string& FilePath)
	{
		SourceData.clear();
		Width = 0;
		Height = 0;

		// COM is initialized per thread, and levels are parsed on a worker.
		// A thread that already lives in another apartment gets
		// RPC_E_CHANGED_MODE back; in that case it must NOT be uninitialized here.
		const HRESULT CoInitResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		const bool bNeedUninit = SUCCEEDED(CoInitResult);

		std::wstring WidePath(FilePath.begin(), FilePath.end());

		// COM objects are held as raw pointers and released by hand, like every
		// other interface pointer in this engine (IDXGI* / ID3D12* included):
		// no ComPtr wrapper, explicit Release on every exit path.
		IWICImagingFactory*    Factory = nullptr;
		IWICBitmapDecoder*     Decoder = nullptr;
		IWICBitmapFrameDecode* Frame = nullptr;
		IWICFormatConverter*   Converter = nullptr;

		bool bOk = false;
		if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&Factory)))
			&& SUCCEEDED(Factory->CreateDecoderFromFilename(WidePath.c_str(), nullptr, GENERIC_READ,
				WICDecodeMetadataCacheOnDemand, &Decoder))
			&& SUCCEEDED(Decoder->GetFrame(0, &Frame))
			&& SUCCEEDED(Factory->CreateFormatConverter(&Converter))
			// Convert everything to RGBA8 so the GPU side never has to know the
			// source format (paletted / grayscale / 24bpp PNG are all handled here).
			&& SUCCEEDED(Converter->Initialize(Frame, GUID_WICPixelFormat32bppRGBA,
				WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom)))
		{
			UINT ImageWidth = 0;
			UINT ImageHeight = 0;
			if (SUCCEEDED(Converter->GetSize(&ImageWidth, &ImageHeight)) && ImageWidth > 0 && ImageHeight > 0)
			{
				const UINT RowPitch = ImageWidth * 4;
				SourceData.resize((size_t)RowPitch * ImageHeight);
				if (SUCCEEDED(Converter->CopyPixels(nullptr, RowPitch, (UINT)SourceData.size(), SourceData.data())))
				{
					Width = ImageWidth;
					Height = ImageHeight;
					SourcePath = FilePath;
					bOk = true;
				}
			}
		}

		if (!bOk)
		{
			SourceData.clear();
			OutputDebugStringA(("CTexture2D::LoadSourceFromFile: failed to decode " + FilePath + "\n").c_str());
		}

		SafeRelease(Converter);
		SafeRelease(Frame);
		SafeRelease(Decoder);
		SafeRelease(Factory);

		if (bNeedUninit)
		{
			CoUninitialize();
		}
		return bOk;
	}

	bool CTexture2D::InitRHI()
	{
		ReleaseRHI();

		if (SourceData.empty() || 0 == Width || 0 == Height)
		{
			return false;
		}

		RHITexture = new CRHITexture2D();
		if (!RHITexture->Create(DXGI_FORMAT_R8G8B8A8_UNORM, Width, Height, SourceData.data(), Width * 4))
		{
			SafeDelete(RHITexture);
			OutputDebugStringA(("CTexture2D::InitRHI: upload failed for " + SourcePath + "\n").c_str());
			return false;
		}

		// The GPU owns a private copy now: keeping the CPU source around would only
		// be useful for rebuilding the resource after a device loss / mip re-stream.
		SourceData.clear();
		SourceData.shrink_to_fit();
		return true;
	}

	void CTexture2D::ReleaseRHI()
	{
		SafeDelete(RHITexture);
	}

	bool CTexture2D::HasValidRHI() const
	{
		return nullptr != RHITexture && RHITexture->IsValid();
	}
}
