// Filter "Frame contexts"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_RenderTexture.hpp"

#include <Common_Color.hpp>
#include <Common_Math.hpp>

#include <Graphics_RenderTexture.hpp>

#include <d3d12.h>

#include <array>
#include <cstddef>

// https://alextardif.com/D3D11To12P3.html
// https://alextardif.com/DX12Tutorial.html

namespace RoseGold::DirectX12
{
	class Device;

	class FrameContext
	{
		static constexpr std::size_t ourMaxQueuedBarriers = 16u;

	public:
		static constexpr std::size_t MaxTextureSubresourceCount = 32;
		using SubresourceLayouts = std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, MaxTextureSubresourceCount>;

	public:
		FrameContext(Device& aDevice, D3D12_COMMAND_LIST_TYPE aCommandType);
		virtual ~FrameContext() = default;

		D3D12_COMMAND_LIST_TYPE GetCommandType() const { return myCommandType; }
		ID3D12GraphicsCommandList* GetCommandList() { return myCommandList.Get(); }

		virtual void Reset();
		void AddBarrier(GPUResource& aResource, D3D12_RESOURCE_STATES aNewState);
		void FlushBarriers();
		void CopyResource(const GPUResource& aSource, GPUResource& aDestination);
		void CopyBufferRegion(const GPUResource& aSource, std::size_t aSourceOffset, GPUResource& aDestination, std::size_t aDestinationOffset, std::size_t aByteCountToCopy);
		void CopyTextureRegion(GPUResource& aSource, std::size_t aSourceOffset, SubresourceLayouts& someSubResourceLayouts, std::uint32_t aSubresourceCount, GPUResource& aDestination);

	protected:
		void BindDescriptorHeaps();

		Device& myDevice;

		D3D12_COMMAND_LIST_TYPE myCommandType;
		ComPtr<ID3D12GraphicsCommandList6> myCommandList;
		ComPtr<ID3D12CommandAllocator> myCommandAllocator;

		RenderPassDescriptorHeap* myCurrentFrameHeap;

		std::array<D3D12_RESOURCE_BARRIER, ourMaxQueuedBarriers> myResourceBarrierQueue;
		std::size_t myQueuedBarriers = 0;
	};

	class UploadContext final : public FrameContext
	{
	public:
		struct BufferUpload
		{
			std::shared_ptr<GPUResource> Resource;
			std::unique_ptr<std::uint8_t> BufferData;
			std::size_t BufferSize = 0;
		};

		struct TextureUpload
		{
			std::shared_ptr<GPUResource> Resource;
			std::unique_ptr<std::uint8_t> BufferData;
			std::uint64_t BufferSize = 0;
			std::uint32_t SubresourceCount = 0;
			SubresourceLayouts SubresourceLayouts;
		};

	public:
		UploadContext(Device& aDevice);

		//void AddUpload(Buffer);
		TextureUpload& AddTextureUpload();
		void ProcessUploads();
		void ResolveUploads();

	private:
		std::unique_ptr<UploadBuffer> myBufferUploadHeap;
		std::unique_ptr<UploadBuffer> myTextureUploadHeap;

		std::vector<std::shared_ptr<GPUResource>> myBufferUploadsInProgress;
		std::vector<std::shared_ptr<GPUResource>> myTextureUploadsInProgress;

		std::vector<BufferUpload> myBufferUploads;
		std::vector<TextureUpload> myTextureUploads;
	};

	class FrameGraphicsContext final : public FrameContext
	{
	public:
		FrameGraphicsContext(Device& aDevice);

		void Reset() override;

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
		void SetPipelineResource(const VertexBuffer& aBuffer);
		void SetPipelineResource(RootParameterUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, void* someData, std::size_t aDataSize);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY aTopology);
		void SetScissorRect(const Math::RectangleT<int>& aRectangle);
		void SetStencilRef(std::uint32_t aStencilRef);
		void SetRenderTargets(const std::vector<RenderTarget*>& someTargets, RenderTarget* aDepthTarget);
		void SetViewportAndScissorRect(const Size& aScreenSize);
		void SetViewport(const Math::Rectangle& aRectangle);

	private:
		inline std::uint32_t GetGroupCount(std::uint32_t threadCount, std::uint32_t groupSize) { return (threadCount + groupSize - 1) / groupSize; }

		PipelineState* myCurrentPipelineState;
		std::vector<std::shared_ptr<ConstantBuffer>> myFrameConstantBuffers;
	};
}