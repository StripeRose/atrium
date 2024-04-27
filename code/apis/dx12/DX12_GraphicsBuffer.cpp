// Filter "Resources"

#include "stdafx.hpp"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_MemoryAlignment.hpp"

namespace RoseGold::DirectX12
{
	GraphicsBuffer::GraphicsBuffer(Device& aDevice, std::uint32_t aBufferSize, D3D12_RESOURCE_STATES aUsageState, D3D12_HEAP_TYPE aHeapType)
	{
		myBufferSize = aBufferSize;
		myResource = CreateResource(aDevice, myBufferSize, aUsageState, aHeapType);
		myUsageState = aUsageState;
	}

	ComPtr<ID3D12Resource> GraphicsBuffer::CreateResource(Device& aDevice, std::uint32_t anAlignedSize, D3D12_RESOURCE_STATES aUsageState, D3D12_HEAP_TYPE aHeapType)
	{
		D3D12_RESOURCE_DESC bufferDesc;
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = anAlignedSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES uploadHeapProperties;
		uploadHeapProperties.Type = aHeapType;
		uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProperties.CreationNodeMask = 0;
		uploadHeapProperties.VisibleNodeMask = 0;

		ComPtr<ID3D12Resource> bufferResource;
		if (!VerifyAction(aDevice.GetDevice()->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			aUsageState,
			NULL,
			IID_PPV_ARGS(bufferResource.ReleaseAndGetAddressOf())),
			"Create graphic buffer resource."))
			return nullptr;

		return bufferResource;
	}

	VertexBuffer::VertexBuffer(Device& aDevice, std::uint32_t aVertexCount, std::uint32_t aVertexStride)
		: GraphicsBuffer(aDevice, aVertexCount * aVertexStride, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD)
	{
		myResource->SetName(L"Vertex buffer");
		myBufferView.StrideInBytes = aVertexStride;
		myBufferView.SizeInBytes = (aVertexCount * aVertexStride);
		myBufferView.BufferLocation = GetGPUAddress();
	}

	void VertexBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		void* mappedBuffer;
		if (!VerifyAction(myResource->Map(0, nullptr, &mappedBuffer), "Mapping vertex buffer to CPU."))
			return;

		std::memcpy(mappedBuffer, aDataPtr, aDataSize);
		myResource->Unmap(0, nullptr);
		myBufferView.SizeInBytes = aDataSize;
		myIsReady = true;
	}

	IndexBuffer::IndexBuffer(Device& aDevice, std::uint32_t anIndexCount)
		: GraphicsBuffer(aDevice, anIndexCount * sizeof(std::uint32_t), D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_HEAP_TYPE_UPLOAD)
	{
		myResource->SetName(L"Index buffer");
		myBufferView.SizeInBytes = anIndexCount * sizeof(std::uint32_t);
		myBufferView.Format = DXGI_FORMAT_R32_UINT;
		myBufferView.BufferLocation = GetGPUAddress();
	}

	void IndexBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		void* mappedBuffer;
		if (!VerifyAction(myResource->Map(0, nullptr, &mappedBuffer), "Mapping index buffer to CPU."))
			return;

		std::memcpy(mappedBuffer, aDataPtr, aDataSize);
		myResource->Unmap(0, nullptr);
		myBufferView.SizeInBytes = aDataSize;
		myIsReady = true;
	}

	ConstantBuffer::ConstantBuffer(Device& aDevice, std::uint32_t aBufferSize)
		: GraphicsBuffer(aDevice, Align<std::uint32_t>(aBufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD)
	{
		myResource->SetName(L"Constant buffer");
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor = { };
		constantBufferViewDescriptor.BufferLocation = myResource->GetGPUVirtualAddress();
		constantBufferViewDescriptor.SizeInBytes = myBufferSize;

		myConstantBufferViewHandle = aDevice.GetDescriptorHeapManager().GetConstantBufferViewHeap().GetNewHeapHandle();
		aDevice.GetDevice()->CreateConstantBufferView(&constantBufferViewDescriptor, myConstantBufferViewHandle.GetCPUHandle());

		myMappedBuffer = nullptr;
		myIsReady = SUCCEEDED(myResource->Map(0, nullptr, &myMappedBuffer));
	}

	ConstantBuffer::~ConstantBuffer()
	{
		if (myMappedBuffer != nullptr)
		{
			myResource->Unmap(0, nullptr);
			myMappedBuffer = nullptr;
		}
	}

	void ConstantBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		Debug::Assert(aDataSize <= myBufferSize, "Data size(%i bytes) less or equal to buffer size(%i bytes).", aDataSize, myBufferSize);
		std::memcpy(myMappedBuffer, aDataPtr, aDataSize);
	}

	UploadBuffer::UploadBuffer(Device& aDevice, std::uint32_t aBufferSize)
		: GraphicsBuffer(aDevice, Align<std::uint32_t>(aBufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD)
	{
		myResource->SetName(L"Upload buffer");
		myMappedBuffer = nullptr;
		myIsReady = SUCCEEDED(myResource->Map(0, nullptr, &myMappedBuffer));
	}

	UploadBuffer::~UploadBuffer()
	{
		if (myMappedBuffer != nullptr)
		{
			myResource->Unmap(0, nullptr);
			myMappedBuffer = nullptr;
		}
	}

	void UploadBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		Debug::Assert(aDataSize <= myBufferSize, "Data size(%i bytes) less or equal to buffer size(%i bytes).", aDataSize, myBufferSize);
		std::memcpy(myMappedBuffer, aDataPtr, aDataSize);
	}

	void UploadBuffer::SetData(std::size_t aDestinationOffset, const void* aDataPtr, std::uint32_t aDataSize)
	{
		Debug::Assert(aDataSize <= (myBufferSize - aDestinationOffset), "Data size(%i bytes) less or equal to the remaining buffer size(%i bytes).", aDataSize, myBufferSize - aDestinationOffset);

		std::uint8_t* mappedData = static_cast<std::uint8_t*>(myMappedBuffer);
		std::memcpy(&mappedData[aDestinationOffset], aDataPtr, aDataSize);
	}
}
