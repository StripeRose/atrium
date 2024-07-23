// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"

#include "Core_GraphicsBuffer.hpp"

#include <d3d12.h>

namespace Atrium::DirectX12
{
	class Device;
	class GraphicsBuffer : public Core::GraphicsBuffer, public GPUResource
	{
	public:
		GraphicsBuffer(Device& aDevice, std::uint32_t aBufferSize, D3D12_RESOURCE_STATES aUsageState, D3D12_HEAP_TYPE aHeapType);

		// Implementing GraphicsBuffer
	public:
		void* GetNativeBufferPtr() override { return myResource.Get(); }
		void SetName(const wchar_t* aName) override { myResource->SetName(aName); }

	protected:
		std::uint32_t myBufferSize;

	private:
		static ComPtr<ID3D12Resource> CreateResource(Device& aDevice, std::uint32_t anAlignedSize, D3D12_RESOURCE_STATES aUsageState, D3D12_HEAP_TYPE aHeapType);
	};

	class VertexBuffer : public GraphicsBuffer
	{
	public:
		VertexBuffer(Device& aDevice, std::uint32_t aVertexCount, std::uint32_t aVertexStride);

		const D3D12_VERTEX_BUFFER_VIEW& GetBufferView() const { return myBufferView; }

		// Implementing GraphicsBuffer
	public:
		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset) override;

		std::uint32_t GetCount() const override { return myBufferView.SizeInBytes / myBufferView.StrideInBytes; }
		std::uint32_t GetStride() const override { return myBufferView.StrideInBytes; }
		Target GetTarget() const override { return Target::Vertex; }

	private:
		D3D12_VERTEX_BUFFER_VIEW myBufferView;
	};

	class IndexBuffer : public GraphicsBuffer
	{
	public:
		IndexBuffer(Device& aDevice, std::uint32_t anIndexCount);

		const D3D12_INDEX_BUFFER_VIEW GetBufferView() const { return myBufferView; }

		// Implementing GraphicsBuffer
	public:
		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset) override;

		std::uint32_t GetCount() const override { return myBufferView.SizeInBytes / GetStride(); }
		std::uint32_t GetStride() const override { return sizeof(std::uint32_t); }
		Target GetTarget() const override { return Target::Index; }

	private:
		D3D12_INDEX_BUFFER_VIEW myBufferView;
	};

	class ConstantBuffer : public GraphicsBuffer
	{
	public:
		ConstantBuffer(Device& aDevice, std::uint32_t aBufferSize);
		~ConstantBuffer() override;

		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset) override;
		const DescriptorHeapHandle& GetViewHandle() const { return myConstantBufferViewHandle; }

		std::uint32_t GetCount() const { return 1; }
		std::uint32_t GetStride() const { return myBufferSize; }
		Target GetTarget() const override { return Target::Constant; }

	private:
		void* myMappedBuffer;

		DescriptorHeapHandle myConstantBufferViewHandle;
	};

	class UploadBuffer : public GraphicsBuffer
	{
	public:
		UploadBuffer(Device& aDevice, std::uint32_t aBufferSize);
		~UploadBuffer() override;

		void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset) override;

		std::uint32_t GetCount() const { return 1; }
		std::uint32_t GetStride() const { return myBufferSize; }
		Target GetTarget() const override { return Target::Constant; }

	private:
		void* myMappedBuffer;
	};
}
