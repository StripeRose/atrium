// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"

namespace RoseGold::DirectX12
{
	GraphicsBuffer::GraphicsBuffer(ComPtr<ID3D12Resource> aResource, D3D12_RESOURCE_STATES aUsageState)
	{
		myResource = aResource;
		myUsageState = aUsageState;
	}

	std::uint32_t GraphicsBuffer::Align(std::uint32_t aLocation, std::uint32_t anAlignment)
	{
		return (aLocation + (anAlignment - 1)) & ~(anAlignment - 1);
	}

	ComPtr<ID3D12Resource> GraphicsBuffer::CreateResource(Device& aDevice, std::uint32_t aBufferSize)
	{
		const std::uint32_t alignedSize = Align(aBufferSize);

		D3D12_RESOURCE_DESC bufferDesc;
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = alignedSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES uploadHeapProperties;
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProperties.CreationNodeMask = 0;
		uploadHeapProperties.VisibleNodeMask = 0;

		ComPtr<ID3D12Resource> bufferResource;
		if (!AssertSuccess(aDevice.GetDevice()->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL,
			IID_PPV_ARGS(bufferResource.ReleaseAndGetAddressOf()))))
			return nullptr;

		return bufferResource;
	}

	VertexBuffer::VertexBuffer(Device& aDevice, std::uint32_t aVertexCount, std::uint32_t aVertexStride)
		: GraphicsBuffer(CreateResource(aDevice, aVertexCount * aVertexStride), D3D12_RESOURCE_STATE_GENERIC_READ)
	{
		myBufferView.StrideInBytes = aVertexStride;
		myBufferView.SizeInBytes = (aVertexCount * aVertexStride);
		myBufferView.BufferLocation = GetGPUAddress();

		/*
		* VertexBufferBackgroundUpload *vertexBufferUpload = new VertexBufferBackgroundUpload(this, vertexBuffer, vertexData);
		* mUploadContext->AddBackgroundUpload(vertexBufferUpload);
		*/
	}

	void VertexBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		void* mappedBuffer;
		if (!LogIfError(myResource->Map(0, nullptr, &mappedBuffer), "Mapping vertex buffer to CPU."))
			return;

		std::memcpy(mappedBuffer, aDataPtr, aDataSize);
		myResource->Unmap(0, nullptr);
		myBufferView.SizeInBytes = aDataSize;
	}

	IndexBuffer::IndexBuffer(Device& aDevice, std::uint32_t anIndexCount)
		: GraphicsBuffer(CreateResource(aDevice, anIndexCount * sizeof(std::uint32_t)), D3D12_RESOURCE_STATE_INDEX_BUFFER)
	{
		myBufferView.SizeInBytes = anIndexCount * sizeof(std::uint32_t);
		myBufferView.Format = DXGI_FORMAT_R32_UINT;
		myBufferView.BufferLocation = GetGPUAddress();
	}

	void IndexBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		void* mappedBuffer;
		if (!LogIfError(myResource->Map(0, nullptr, &mappedBuffer), "Mapping index buffer to CPU."))
			return;

		std::memcpy(mappedBuffer, aDataPtr, aDataSize);
		myResource->Unmap(0, nullptr);
		myBufferView.SizeInBytes = aDataSize;
	}

	ConstantBuffer::ConstantBuffer(Device& aDevice, std::uint32_t aBufferSize)
		: GraphicsBuffer(CreateResource(aDevice, aBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ)
	{
		const std::uint32_t alignedSize = Align(aBufferSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor = { };
		constantBufferViewDescriptor.BufferLocation = myResource->GetGPUVirtualAddress();
		constantBufferViewDescriptor.SizeInBytes = alignedSize;

		myConstantBufferViewHandle = aDevice.GetDescriptorHeapManager().GetConstantBufferViewHeap().GetNewHeapHandle();
		aDevice.GetDevice()->CreateConstantBufferView(&constantBufferViewDescriptor, myConstantBufferViewHandle->GetCPUHandle());

		myBufferSize = aBufferSize;
		myMappedBuffer = nullptr;
		myIsReady = SUCCEEDED(myResource->Map(0, nullptr, &myMappedBuffer));
	}

	ConstantBuffer::~ConstantBuffer()
	{
		if (myMappedBuffer == nullptr)
		{
			myResource->Unmap(0, nullptr);
			myMappedBuffer = nullptr;
		}
	}

	void ConstantBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize)
	{
		Debug::Assert(aDataSize >= myBufferSize, "Tried to assign %i bytes to a buffer of size %i.", aDataSize, myBufferSize);
		std::memcpy(myMappedBuffer, aDataPtr, aDataSize);
	}
}
