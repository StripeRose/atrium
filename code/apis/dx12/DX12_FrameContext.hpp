// Filter "Frame contexts"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"

#include <Common_Color.hpp>
#include <Common_Math.hpp>

#include <Graphics_RenderTexture.hpp>

#include <d3d12.h>

#include <array>
#include <cstddef>

namespace RoseGold::DirectX12
{
	class Device;
	class FrameContext
	{
		static constexpr std::size_t ourMaxQueuedBarriers = 16u;

	public:
		FrameContext(Device& aDevice, D3D12_COMMAND_LIST_TYPE aCommandType);
		virtual ~FrameContext() = default;

		D3D12_COMMAND_LIST_TYPE GetCommandType() const { return myCommandType; }
		ID3D12GraphicsCommandList* GetCommandList() { return myCommandList.Get(); }

		void Reset();
		void AddBarrier(GPUResource& aResource, D3D12_RESOURCE_STATES aNewState);
		void FlushBarriers();
		void CopyResource(const GPUResource& aSource, GPUResource& aDestination);
		void CopyBufferRegion(const GPUResource& aSource, std::size_t aSourceOffset, GPUResource& aDestination, std::size_t aDestinationOffset, std::size_t aByteCountToCopy);
		void SetConstantBufferData(void* someData, std::size_t aDataSize);

	protected:
		void BindDescriptorHeaps();

		Device& myDevice;

		D3D12_COMMAND_LIST_TYPE myCommandType;
		ComPtr<ID3D12GraphicsCommandList6> myCommandList;
		ComPtr<ID3D12CommandAllocator> myCommandAllocator;

		RenderPassDescriptorHeap* myCurrentFrameHeap;

		std::array<D3D12_RESOURCE_BARRIER, ourMaxQueuedBarriers> myResourceBarrierQueue;
		std::size_t myQueuedBarriers = 0;

		std::vector<std::shared_ptr<ConstantBuffer>> myFrameConstantBuffers;
	};

	class FrameGraphicsContext final : public FrameContext
	{
	public:
		FrameGraphicsContext(Device& aDevice);

		void ClearColor(const Core::Graphics::RenderTexture& aTarget, Color aClearColor);
		void ClearDepth(const Core::Graphics::RenderTexture& aTarget, float aDepth, std::uint8_t aStencil);

		void DisableScissorRect();

		void Dispatch(std::uint32_t aGroupCountX, std::uint32_t aGroupCountY, std::uint32_t aGroupCountZ);
		void Dispatch1D(std::uint32_t aThreadCountX, std::uint32_t aGroupSizeX);
		void Dispatch2D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY);
		void Dispatch3D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aThreadCountZ, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY, std::uint32_t aGroupSizeZ);

		void Draw(std::uint32_t aVertexCount, std::uint32_t aVertexStartOffset);
		void DrawIndexed(std::uint32_t anIndexCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation);
		void DrawInstanced(std::uint32_t aVertexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartVertexLocation, std::uint32_t aStartInstanceLocation);
		void DrawIndexedInstanced(std::uint32_t anIndexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation, std::uint32_t aStartInstanceLocation);

		void SetBlendFactor(Color aBlendFactor);
		void SetPipelineState(PipelineState& aPipelineState);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY aTopology);
		void SetScissorRect(const Math::RectangleT<int>& aRectangle);
		void SetStencilRef(std::uint32_t aStencilRef);
		void SetVertexBuffer(unsigned int aStartSlot, const VertexBuffer& aBuffer);
		void SetViewportAndScissorRect(const Size& aScreenSize);
		void SetViewport(const Math::Rectangle& aRectangle);

	private:
		inline std::uint32_t GetGroupCount(std::uint32_t threadCount, std::uint32_t groupSize) { return (threadCount + groupSize - 1) / groupSize; }
	};
}