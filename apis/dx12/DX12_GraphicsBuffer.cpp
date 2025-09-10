// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Manager.hpp"
#include "DX12_MemoryAlignment.hpp"

namespace Atrium::DirectX12
{
	BackendGraphicsBuffer::BackendGraphicsBuffer(Device& aDevice, GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
		: myCount(aCount)
		, myStride(aStride)
		, myMappedBuffer(nullptr)
	{
		static constexpr GraphicsBuffer::Target SupportedTargets
			= GraphicsBuffer::Target::Constant
			| GraphicsBuffer::Target::Index
			| GraphicsBuffer::Target::Vertex
			;

		Debug::Assert(
			(aTarget & ~SupportedTargets) == GraphicsBuffer::Target::None,
			"Supported targets: Constant, Index, Vertex"
		);

		const std::uint32_t alignedSize =
			(aTarget & GraphicsBuffer::Target::Constant) != GraphicsBuffer::Target::None
			? Align<std::uint32_t>(aCount * aStride, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)
			: aCount * aStride;

		D3D12_RESOURCE_STATES usageState = D3D12_RESOURCE_STATE_GENERIC_READ;

		if ((aTarget & GraphicsBuffer::Target::Index) != GraphicsBuffer::Target::None)
			usageState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;

		CreateResource(aDevice, usageState, alignedSize);

		if ((aTarget & GraphicsBuffer::Target::Vertex) != GraphicsBuffer::Target::None)
			CreateVertexView(aCount, aStride);

		if ((aTarget & GraphicsBuffer::Target::Index) != GraphicsBuffer::Target::None)
			CreateIndexView(aCount, aStride);

		if ((aTarget & GraphicsBuffer::Target::Constant) != GraphicsBuffer::Target::None)
			CreateConstantView(aDevice, alignedSize);
	}

	BackendGraphicsBuffer::~BackendGraphicsBuffer()
	{
		Unmap();
	}

	void BackendGraphicsBuffer::Map()
	{
		if (myMappedBuffer != nullptr)
		{
			Debug::LogWarning("Graphics buffer was already mapped. It may be unintended to call this function on a mapped buffer.");
			return;
		}

		Debug::Verify(
			myResource->GetResource()->Map(0, nullptr, &myMappedBuffer),
			"Mapping vertex buffer to CPU.");
	}

	void BackendGraphicsBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset)
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

	void BackendGraphicsBuffer::Unmap()
	{
		if (myMappedBuffer == nullptr)
			return;

		myResource->GetResource()->Unmap(0, nullptr);
		myMappedBuffer = nullptr;
	}

	void BackendGraphicsBuffer::CreateConstantView(Device& aDevice, std::uint32_t anAlignedSize)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor = { };
		constantBufferViewDescriptor.BufferLocation = myResource->GetResource()->GetGPUVirtualAddress();
		constantBufferViewDescriptor.SizeInBytes = anAlignedSize;

		myConstantViewDescriptor = aDevice.GetDescriptorHeapManager().GetConstantBufferViewHeap().GetNewHeapHandle();
		aDevice.GetDevice()->CreateConstantBufferView(&constantBufferViewDescriptor, myConstantViewDescriptor.GetCPUHandle());
	}

	void BackendGraphicsBuffer::CreateIndexView(std::uint32_t aCount, std::uint32_t aStride)
	{
		Debug::Assert(sizeof(std::uint32_t) == aStride, "Assumes indices to be uint32.");

		D3D12_INDEX_BUFFER_VIEW indexView;
		indexView.SizeInBytes = aCount * sizeof(std::uint32_t);
		indexView.Format = DXGI_FORMAT_R32_UINT;
		indexView.BufferLocation = myResource->GetGPUAddress();
		myIndexView = indexView;
	}

	void BackendGraphicsBuffer::CreateResource(Device& aDevice, D3D12_RESOURCE_STATES aUsageState, std::uint32_t anAlignedSize)
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

	void BackendGraphicsBuffer::CreateVertexView(std::uint32_t aCount, std::uint32_t aStride)
	{
		D3D12_VERTEX_BUFFER_VIEW vertexView;
		vertexView.StrideInBytes = aStride;
		vertexView.SizeInBytes = (aCount * aStride);
		vertexView.BufferLocation = myResource->GetGPUAddress();
		myVertexView = vertexView;
	}

	GraphicsBuffer::GraphicsBuffer(DirectX12API& anAPI, GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
		: myAPI(anAPI)
		, myLastWrittenBuffer(nullptr)
		, myTarget(aTarget)
		, myCount(aCount)
		, myStride(aStride)
	{
		myBuffers.resize(DirectX12API::GetFramesInFlightAmount());
	}

	void* GraphicsBuffer::GetNativeBufferPtr()
	{
		if (std::shared_ptr<GPUResource> resource = GetBufferForRead().GetResource())
			return resource->GetResource().Get();
		else
			return nullptr;
	}

	void GraphicsBuffer::SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset)
	{
		BackendGraphicsBuffer& frameBuffer = GetBufferForWrite();
		frameBuffer.Map();
		frameBuffer.SetData(aDataPtr, aDataSize, aDestinationOffset);
		frameBuffer.Unmap();
	}

	void GraphicsBuffer::SetName(const wchar_t* aName)
	{
		for (auto& it : myBuffers)
		{
			if (it)
				it->GetResource()->SetName(aName);
		}
	}

	BackendGraphicsBuffer& GraphicsBuffer::GetBufferForRead() const
	{
		if (!myLastWrittenBuffer)
			throw std::runtime_error("Buffer was never written to; There is no buffer to read.");

		return *myLastWrittenBuffer;
	}

	BackendGraphicsBuffer& GraphicsBuffer::GetBufferForWrite()
	{
		const std::uint_least8_t frameInFlight = myAPI.GetFrameInFlight();

		std::shared_ptr<BackendGraphicsBuffer>& frameBuffer = myBuffers.at(frameInFlight);

		if (frameBuffer == nullptr)
			frameBuffer.reset(new BackendGraphicsBuffer(myAPI.GetDevice(), myTarget, myCount, myStride));

		myLastWrittenBuffer = frameBuffer.get();

		return *myLastWrittenBuffer;
	}
}
