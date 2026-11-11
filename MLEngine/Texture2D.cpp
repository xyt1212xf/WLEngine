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

	bool CTexture2D::DecodeFileToRGBA(const std::string& FilePath, std::vector<uint8>& OutPixels, uint32& OutWidth, uint32& OutHeight)
	{
		OutPixels.clear();
		OutWidth = 0;
		OutHeight = 0;

		const HRESULT CoInitResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		const bool bNeedUninit = SUCCEEDED(CoInitResult);

		std::wstring WidePath(FilePath.begin(), FilePath.end());

		IWICImagingFactory* Factory = nullptr;
		IWICBitmapDecoder* Decoder = nullptr;
		IWICBitmapFrameDecode* Frame = nullptr;
		IWICFormatConverter* Converter = nullptr;

		bool bOk = false;
		if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&Factory)))
			&& SUCCEEDED(Factory->CreateDecoderFromFilename(WidePath.c_str(), nullptr, GENERIC_READ,
				WICDecodeMetadataCacheOnDemand, &Decoder))
			&& SUCCEEDED(Decoder->GetFrame(0, &Frame))
			&& SUCCEEDED(Factory->CreateFormatConverter(&Converter))
			&& SUCCEEDED(Converter->Initialize(Frame, GUID_WICPixelFormat32bppRGBA,
				WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom)))
		{
			UINT ImageWidth = 0;
			UINT ImageHeight = 0;
			if (SUCCEEDED(Converter->GetSize(&ImageWidth, &ImageHeight)) && ImageWidth > 0 && ImageHeight > 0)
			{
				const UINT RowPitch = ImageWidth * 4;
				OutPixels.resize((size_t)RowPitch * ImageHeight);
				if (SUCCEEDED(Converter->CopyPixels(nullptr, RowPitch, (UINT)OutPixels.size(), OutPixels.data())))
				{
					OutWidth = ImageWidth;
					OutHeight = ImageHeight;
					bOk = true;
				}
				else
				{
					OutPixels.clear();
					OutputDebugStringA(("CTexture2D::DecodeFileToRGBA: failed to decode " + FilePath + "\n").c_str());
				}
			}
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

	bool CTexture2D::LoadSourceFromFile(const std::string& FilePath)
	{
		SourceData.clear();
		Width = 0;
		Height = 0;

		if (!DecodeFileToRGBA(FilePath, SourceData, Width, Height))
		{
			SourceData.clear();
			Width = 0;
			Height = 0;
			return false;
		}
		SourcePath = FilePath;
		return true;
	}

	void CTexture2D::SetLayerSlices(std::vector<std::vector<uint8>>&& Slices, uint32 InWidth, uint32 InHeight)
	{
		LayerSlices = std::move(Slices);
		Width = InWidth;
		Height = InHeight;
		SourceData.clear();
	}

	bool CTexture2D::InitRHI()
	{
		ReleaseRHI();

		// Layer-array path: N RGBA8 slices packed into one Texture2DArray.
		if (!LayerSlices.empty())
		{
			if (0 == Width || 0 == Height || LayerSlices.size() > 16)
			{
				return false;
			}

			std::vector<D3D12_SUBRESOURCE_DATA> Subresources;
			Subresources.reserve(LayerSlices.size());
			for (const std::vector<uint8>& Slice : LayerSlices)
			{
				if (Slice.size() != (size_t)Width * Height * 4)
				{
					OutputDebugStringA("CTexture2D::InitRHI: layer slice size mismatch\n");
					return false;
				}
				D3D12_SUBRESOURCE_DATA Data = {};
				Data.pData = Slice.data();
				Data.RowPitch = Width * 4;
				Data.SlicePitch = (UINT64)Width * Height * 4;
				Subresources.push_back(Data);
			}

			RHITexture = new CRHITexture2D();
			FTextureRHIInfo TextureInfo;
			TextureInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			TextureInfo.Width = Width;
			TextureInfo.Height = Height;
			TextureInfo.ArraySize = (uint16)LayerSlices.size();
			if (!RHITexture->Create(TextureInfo, Subresources.data(), (uint32)Subresources.size()))
			{
				SafeDelete(RHITexture);
				OutputDebugStringA("CTexture2D::InitRHI: layer array upload failed\n");
				return false;
			}

			LayerSlices.clear();
			LayerSlices.shrink_to_fit();
			return true;
		}

		if (SourceData.empty() || 0 == Width || 0 == Height)
		{
			return false;
		}

		RHITexture = new CRHITexture2D();

		FTextureRHIInfo TextureInfo;
		TextureInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureInfo.Width = Width;
		TextureInfo.Height = Height;

		D3D12_SUBRESOURCE_DATA Data;
		Data.pData = SourceData.data();
		Data.RowPitch = Width * 4;
        Data.SlicePitch = (UINT64)Width * Height * 4;

		if (!RHITexture->Create(TextureInfo, &Data, 1))
		{
			SafeDelete(RHITexture);
			OutputDebugStringA(("CTexture2D::InitRHI: upload failed for " + SourcePath + "\n").c_str());
			return false;
		}

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
