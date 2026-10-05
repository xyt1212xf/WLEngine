#include "DX12RHIbuffer.h"
#include "GraphicPlug.h"
#include "MLEngine.h"

namespace ML
{
	CDX12RHIBuffer::CDX12RHIBuffer(ID3D12Resource* InResource)
	: Resource(InResource)
	{

	}


	CDX12RHIBuffer::~CDX12RHIBuffer()
	{
		Release();
	}
	
	bool CDX12RHIBuffer::Create(EBufferType InType, uint32 InSize, uint32 InStride, const void* InitData)
	{
		Release();

		Type = InType;
		Size = InSize;
		Stride = InStride;
		CDX12RHIDevice* pRHIDevice = GEngine->GetPlugs<CGraphicPlug>()->GetDevice();

		if (ID3D12Device* Device = pRHIDevice->GetDevice())
		{
			// 1. 创建默认堆资源（GPU 可读）
			D3D12_HEAP_PROPERTIES HeapProps = {};
			HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

			D3D12_RESOURCE_DESC ResDesc = {};
			ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResDesc.Alignment = 0;
			ResDesc.Width = Size;
			ResDesc.Height = 1;
			ResDesc.DepthOrArraySize = 1;
			ResDesc.MipLevels = 1;
			ResDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResDesc.SampleDesc.Count = 1;
			ResDesc.SampleDesc.Quality = 0;
			ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			// 有初始数据时，默认堆先处于 COPY_DEST 状态，拷贝完成后再屏障转换到 GENERIC_READ
			D3D12_RESOURCE_STATES InitialState = (InitData != nullptr)
				? D3D12_RESOURCE_STATE_COPY_DEST
				: D3D12_RESOURCE_STATE_GENERIC_READ;

			HRESULT hr = Device->CreateCommittedResource(
				&HeapProps,
				D3D12_HEAP_FLAG_NONE,
				&ResDesc,
				InitialState,
				nullptr,
				IID_PPV_ARGS(&Resource)
			);

			if (FAILED(hr))
			{
				return false;
			}
			// 2. 如果有初始数据，创建上传堆并拷贝
			if (InitData != nullptr)
			{
				D3D12_HEAP_PROPERTIES UploadHeapProps = {};
				UploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

				D3D12_RESOURCE_DESC UploadResDesc = {};
				UploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				UploadResDesc.Alignment = 0;
				UploadResDesc.Width = Size;
				UploadResDesc.Height = 1;
				UploadResDesc.DepthOrArraySize = 1;
				UploadResDesc.MipLevels = 1;
				UploadResDesc.Format = DXGI_FORMAT_UNKNOWN;
				UploadResDesc.SampleDesc.Count = 1;
				UploadResDesc.SampleDesc.Quality = 0;
				UploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				UploadResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				hr = Device->CreateCommittedResource(
					&UploadHeapProps,
					D3D12_HEAP_FLAG_NONE,
					&UploadResDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&UploadBuffer)
				);
				if (FAILED(hr))
				{
					Release();
					return false;
				}

				// 3. 映射并拷贝数据
				hr = UploadBuffer->Map(0, nullptr, &MappedData);
				if (FAILED(hr))
				{
					Release();
					return false;
				}

				memcpy(MappedData, InitData, Size);
				UploadBuffer->Unmap(0, nullptr);
				MappedData = nullptr;

				// 4. 从上传堆拷贝到默认堆：用专用的临时 command list 录制，
				//    立刻提交执行并等待 GPU 完成（主渲染 cmd list 此时处于 closed 状态，不能复用）
				ID3D12CommandAllocator* UploadAllocator = nullptr;
				ID3D12GraphicsCommandList* UploadList = nullptr;

				if (FAILED(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&UploadAllocator))) ||
					FAILED(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, UploadAllocator, nullptr, IID_PPV_ARGS(&UploadList))))
				{
					UploadAllocator->Release();
					Release();
					return false;
				}

				UploadList->CopyBufferRegion(Resource, 0, UploadBuffer, 0, Size);

				// 5. 资源屏障：默认堆从 COPY_DEST 到 GENERIC_READ
				D3D12_RESOURCE_BARRIER Barrier = {};
				Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				Barrier.Transition.pResource = Resource;
				Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

				UploadList->ResourceBarrier(1, &Barrier);

				// 内部会 Close / Execute / Signal / 阻塞等待 fence，并释放 UploadList
				pRHIDevice->ExecuteUploadAndWait(UploadList);

				UploadAllocator->Release();

				// GPU 拷贝已确认完成，上传堆可以立即释放
				SafeRelease(UploadBuffer);
			}
		}
		return true;
	}

	void CDX12RHIBuffer::Release()
	{
		if (MappedData)
		{
			if (UploadBuffer)
			{
				UploadBuffer->Unmap(0, nullptr);
			}
			MappedData = nullptr;
		}

		SafeRelease(UploadBuffer);
		SafeRelease(Resource);
		Size = 0;
		Stride = 0;
	}

	D3D12_VERTEX_BUFFER_VIEW CDX12RHIBuffer::GetVertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW View = {};
		View.BufferLocation = Resource ? Resource->GetGPUVirtualAddress() : 0;
		View.SizeInBytes = Size;
		View.StrideInBytes = Stride;
		return View;
	}

	D3D12_INDEX_BUFFER_VIEW CDX12RHIBuffer::GetIndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW View = {};
		View.BufferLocation = Resource ? Resource->GetGPUVirtualAddress() : 0;
		View.SizeInBytes = Size;
		View.Format = (Stride == sizeof(uint16)) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		return View;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC CDX12RHIBuffer::GetConstantBufferViewDesc() const
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc = {};
		Desc.BufferLocation = Resource ? Resource->GetGPUVirtualAddress() : 0;
		Desc.SizeInBytes = Size;
		return Desc;
	}

}