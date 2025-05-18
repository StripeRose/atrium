// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"

#include "Atrium_GraphicsBuffer.hpp"

#include <d3d12.h>

#include <vector>

namespace Atrium::DirectX12
{
	class Device;
	class DirectX12API;

	class BackendGraphicsBuffer
	{
	public:
		BackendGraphicsBuffer(Device& aDevice, Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride);

		~BackendGraphicsBuffer();

		std::uint32_t GetCount() const { return myCount; }
		std::uint32_t GetStride() const { return myStride; }

		const DescriptorHeapHandle GetConstantViewHandle() const { return myConstantViewDescriptor; }
		std::optional<D3D12_INDEX_BUFFER_VIEW> GetIndexView() const { return myIndexView; }
		std::optional<D3D12_VERTEX_BUFFER_VIEW> GetVertexView() const { return myVertexView; }

		std::shared_ptr<GPUResource> GetResource() { return myResource; }

		void Map();
		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset);
		void Unmap();

	protected:
		void CreateConstantView(Device& aDevice, std::uint32_t anAlignedSize);
		void CreateIndexView(std::uint32_t aCount, std::uint32_t aStride);
		void CreateResource(Device& aDevice, D3D12_RESOURCE_STATES aUsageState, std::uint32_t anAlignedSize);
		void CreateVertexView(std::uint32_t aCount, std::uint32_t aStride);

	private:
		std::shared_ptr<GPUResource> myResource;

		std::optional<D3D12_INDEX_BUFFER_VIEW> myIndexView;
		std::optional<D3D12_VERTEX_BUFFER_VIEW> myVertexView;
		DescriptorHeapHandle myConstantViewDescriptor;

		void* myMappedBuffer;
		std::uint32_t myCount;
		std::uint32_t myStride;
	};

	class GraphicsBuffer : public Core::GraphicsBuffer
	{
	public:
		GraphicsBuffer(DirectX12API& anAPI, Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride);

		const DescriptorHeapHandle GetConstantViewHandle() const { return GetBufferForRead().GetConstantViewHandle(); }
		std::optional<D3D12_INDEX_BUFFER_VIEW> GetIndexView() const { return GetBufferForRead().GetIndexView(); }
		std::optional<D3D12_VERTEX_BUFFER_VIEW> GetVertexView() const { return GetBufferForRead().GetVertexView(); }

		// Implementing Atrium::GraphicsBuffer
	public:
		std::uint32_t GetCount() const override { return myCount; }
		std::uint32_t GetStride() const override { return myStride; }

		void* GetNativeBufferPtr() override;

		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset) override;
		void SetName(const wchar_t* aName) override;

	private:
		BackendGraphicsBuffer& GetBufferForRead() const;
		BackendGraphicsBuffer& GetBufferForWrite();

		DirectX12API& myAPI;

		std::vector<std::shared_ptr<BackendGraphicsBuffer>> myBuffers;
		BackendGraphicsBuffer* myLastWrittenBuffer;

		Core::GraphicsBuffer::Target myTarget;
		std::uint32_t myCount;
		std::uint32_t myStride;
	};
}
