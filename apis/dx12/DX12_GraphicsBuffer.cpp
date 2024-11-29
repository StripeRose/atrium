// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_MemoryAlignment.hpp"

namespace Atrium::DirectX12
{
	GraphicsBuffer::GraphicsBuffer(std::uint32_t aCount, std::uint32_t aStride)
		: myCount(aCount)
		, myStride(aStride)
	{

	}

	GraphicsBuffer::GraphicsBuffer(Device& aDevice, Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		const std::uint32_t alignedSize = AlignSize(aTarget, aCount, aStride);

		D3D12_RESOURCE_STATES usageState = D3D12_RESOURCE_STATE_GENERIC_READ;

		if ((aTarget & Core::GraphicsBuffer::Target::Index) != Core::GraphicsBuffer::Target::None)
			usageState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;

		CreateResource(aDevice, usageState, alignedSize);

		if ((aTarget & Core::GraphicsBuffer::Target::Vertex) != Core::GraphicsBuffer::Target::None)
			CreateVertexView(aCount, aStride);

		if ((aTarget & Core::GraphicsBuffer::Target::Index) != Core::GraphicsBuffer::Target::None)
			CreateIndexView(aCount, aStride);

		if ((aTarget & Core::GraphicsBuffer::Target::Constant) != Core::GraphicsBuffer::Target::None)
			CreateConstantView(aDevice, alignedSize);
	}

	void* GraphicsBuffer::GetNativeBufferPtr()
	{
		if (myResource)
			return myResource->GetResource().Get();
		else
			return nullptr;
	}

	void GraphicsBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset)
	{
		Map();
		InternalSetData(aDataPtr, aDataSize, aDestinationOffset);
		Unmap();
	}

	void GraphicsBuffer::SetName(const wchar_t* aName)
	{
		if (myResource)
			myResource->SetName(aName);
	}

	std::uint32_t GraphicsBuffer::AlignSize(Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		if ((aTarget & Core::GraphicsBuffer::Target::Constant) != Core::GraphicsBuffer::Target::None)
			return Align<std::uint32_t>(aCount * aStride, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		else
			return aCount * aStride;
	}

	void GraphicsBuffer::CreateConstantView(Device& aDevice, std::uint32_t anAlignedSize)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor = { };
		constantBufferViewDescriptor.BufferLocation = myResource->GetResource()->GetGPUVirtualAddress();
		constantBufferViewDescriptor.SizeInBytes = anAlignedSize;

		myConstantViewDescriptor = aDevice.GetDescriptorHeapManager().GetConstantBufferViewHeap().GetNewHeapHandle();
		aDevice.GetDevice()->CreateConstantBufferView(&constantBufferViewDescriptor, myConstantViewDescriptor.GetCPUHandle());
	}

	void GraphicsBuffer::CreateIndexView(std::uint32_t aCount, std::uint32_t aStride)
	{
		Debug::Assert(sizeof(std::uint32_t) == aStride, "Assumes indices to be uint32.");

		D3D12_INDEX_BUFFER_VIEW indexView;
		indexView.SizeInBytes = aCount * sizeof(std::uint32_t);
		indexView.Format = DXGI_FORMAT_R32_UINT;
		indexView.BufferLocation = myResource->GetGPUAddress();
		myIndexView = indexView;
	}

	void GraphicsBuffer::CreateResource(Device& aDevice, D3D12_RESOURCE_STATES aUsageState, std::uint32_t anAlignedSize)
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

		myResource = aDevice.CreateResource(&bufferDesc, aUsageState, NULL, D3D12_HEAP_TYPE_UPLOAD);
	}

	void GraphicsBuffer::CreateVertexView(std::uint32_t aCount, std::uint32_t aStride)
	{
		D3D12_VERTEX_BUFFER_VIEW vertexView;
		vertexView.StrideInBytes = aStride;
		vertexView.SizeInBytes = (aCount * aStride);
		vertexView.BufferLocation = myResource->GetGPUAddress();
		myVertexView = vertexView;
	}

	void GraphicsBuffer::Map()
	{
		VerifyAction(
			myResource->GetResource()->Map(0, nullptr, &myMappedBuffer),
			"Mapping vertex buffer to CPU.");
	}

	void GraphicsBuffer::Unmap()
	{
		myResource->GetResource()->Unmap(0, nullptr);
		myMappedBuffer = nullptr;
	}

	void GraphicsBuffer::InternalSetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset)
	{
		Debug::Assert(
			(aDataSize + aDestinationOffset) <= (myStride * myCount),
			"Expects aDestinationOffset + aDataSize to not be greater than the buffer size %i, but was %i.",
			myStride * myCount,
			aDestinationOffset + aDataSize);

		std::memcpy((char*)myMappedBuffer + aDestinationOffset, aDataPtr, aDataSize);

		if (myIndexView)
			myIndexView.value().SizeInBytes = aDataSize;

		if (myVertexView)
			myVertexView.value().SizeInBytes = aDataSize;
	}

	UploadBuffer::UploadBuffer(Device& aDevice, std::uint32_t aBufferSize)
		: GraphicsBuffer(1, aBufferSize)
	{
		const std::uint32_t alignedSize = AlignSize(Core::GraphicsBuffer::Target::Constant, 1, aBufferSize);

		CreateResource(aDevice, D3D12_RESOURCE_STATE_GENERIC_READ, alignedSize);

		Map();
	}

	UploadBuffer::~UploadBuffer()
	{
		Unmap();
	}

	void UploadBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset)
	{
		InternalSetData(aDataPtr, aDataSize, aDestinationOffset);
	}
}
