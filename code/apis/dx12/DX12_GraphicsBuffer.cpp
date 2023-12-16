// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"

namespace RoseGold::DirectX12
{
	GraphicsBuffer::GraphicsBuffer(ComPtr<ID3D12Resource> aResource, D3D12_RESOURCE_STATES aUsageState)
		: GPUResource(aResource, aUsageState)
	{ }

	std::uint32_t GraphicsBuffer::Align(std::uint32_t aLocation, std::uint32_t anAlignment)
	{
		return (aLocation + (anAlignment - 1)) & ~(anAlignment - 1);
	}

	VertexBuffer::VertexBuffer(Device& aDevice, std::uint32_t aVertexCount, std::uint32_t aVertexStride)
		: GraphicsBuffer(CreateResource(aDevice, aVertexCount, aVertexStride), D3D12_RESOURCE_STATE_GENERIC_READ)
	{
		myBufferView.StrideInBytes = aVertexStride;
		myBufferView.SizeInBytes = (aVertexCount * aVertexStride);
		myBufferView.BufferLocation = myGPUAddress;

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

	ComPtr<ID3D12Resource> VertexBuffer::CreateResource(Device& aDevice, std::uint32_t aVertexCount, std::uint32_t aVertexStride)
	{
		D3D12_RESOURCE_DESC vertexBufferDesc;
		vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vertexBufferDesc.Alignment = 0;
		vertexBufferDesc.Width = (aVertexCount * aVertexStride);
		vertexBufferDesc.Height = 1;
		vertexBufferDesc.DepthOrArraySize = 1;
		vertexBufferDesc.MipLevels = 1;
		vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		vertexBufferDesc.SampleDesc.Count = 1;
		vertexBufferDesc.SampleDesc.Quality = 0;
		vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES defaultHeapProperties;
		defaultHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		defaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		defaultHeapProperties.CreationNodeMask = 0;
		defaultHeapProperties.VisibleNodeMask = 0;

		ComPtr<ID3D12Resource> bufferResource;
		if (!AssertSuccess(aDevice.GetDevice()->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&vertexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(bufferResource.ReleaseAndGetAddressOf()))))
			return nullptr;
		return bufferResource;
	}

	IndexBuffer::IndexBuffer(Device& aDevice, std::uint32_t anIndexCount)
		: GraphicsBuffer(CreateResource(aDevice, anIndexCount), D3D12_RESOURCE_STATE_INDEX_BUFFER)
	{
		myBufferView.SizeInBytes = anIndexCount * sizeof(std::uint32_t);
		myBufferView.Format = DXGI_FORMAT_R32_UINT;
		myBufferView.BufferLocation = myGPUAddress;
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

	ComPtr<ID3D12Resource> IndexBuffer::CreateResource(Device& aDevice, std::uint32_t anIndexCount)
	{
		D3D12_HEAP_PROPERTIES defaultHeapProperties;
		defaultHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		defaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		defaultHeapProperties.CreationNodeMask = 1;
		defaultHeapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC indexBufferDesc;
		indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		indexBufferDesc.Alignment = 0;
		indexBufferDesc.Width = anIndexCount * sizeof(std::uint32_t);
		indexBufferDesc.Height = 1;
		indexBufferDesc.DepthOrArraySize = 1;
		indexBufferDesc.MipLevels = 1;
		indexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		indexBufferDesc.SampleDesc.Count = 1;
		indexBufferDesc.SampleDesc.Quality = 0;
		indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		indexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPtr<ID3D12Resource> bufferResource;
		if (!AssertSuccess(aDevice.GetDevice()->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(bufferResource.ReleaseAndGetAddressOf()))))
			return nullptr;
		return bufferResource;
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
		Debug::Assert(aDataSize <= myBufferSize, "Tried to assign %i bytes to a buffer of size %i.", aDataSize, myBufferSize);
		std::memcpy(myMappedBuffer, aDataPtr, aDataSize);
	}

	ComPtr<ID3D12Resource> ConstantBuffer::CreateResource(Device& aDevice, std::uint32_t aBufferSize)
	{
		const std::uint32_t alignedSize = Align(aBufferSize);

		D3D12_RESOURCE_DESC constantBufferDesc;
		constantBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		constantBufferDesc.Alignment = 0;
		constantBufferDesc.Width = alignedSize;
		constantBufferDesc.Height = 1;
		constantBufferDesc.DepthOrArraySize = 1;
		constantBufferDesc.MipLevels = 1;
		constantBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		constantBufferDesc.SampleDesc.Count = 1;
		constantBufferDesc.SampleDesc.Quality = 0;
		constantBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		constantBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

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
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			NULL,
			IID_PPV_ARGS(bufferResource.ReleaseAndGetAddressOf()))))
			return nullptr;

		return bufferResource;
	}
}
