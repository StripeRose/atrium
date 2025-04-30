// Filter "Frame contexts"

#pragma once

#include "Atrium_Color.hpp"
#include "Atrium_FrameContext.hpp"
#include "Atrium_Math.hpp"
#include "Atrium_RenderTexture.hpp"

#include "DX12_CommandQueue.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_RenderTexture.hpp"

#include <d3d12.h>

#include <array>
#include <cstddef>

// https://alextardif.com/D3D11To12P3.html
// https://alextardif.com/DX12Tutorial.html

namespace Atrium::DirectX12
{
	class Device;

	class FrameContext
	{
		static constexpr std::size_t ourMaxQueuedBarriers = 16u;

	public:
		static constexpr std::size_t MaxTextureSubresourceCount = 32;
		using SubresourceLayouts = std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, MaxTextureSubresourceCount>;

	public:
		FrameContext(Device& aDevice, CommandQueue& aCommandQueue);
		virtual ~FrameContext() = default;

		D3D12_COMMAND_LIST_TYPE GetCommandType() const { return myCommandType; }
		ID3D12GraphicsCommandList* GetCommandList() { return myCommandList.Get(); }

		virtual void Reset(const std::uint_least8_t& aFrameInFlight);
		void AddBarrier(GPUResource& aResource, D3D12_RESOURCE_STATES aNewState);
		void FlushBarriers();
		void CopyResource(const GPUResource& aSource, GPUResource& aDestination);
		void CopyBufferRegion(const GPUResource& aSource, std::size_t aSourceOffset, GPUResource& aDestination, std::size_t aDestinationOffset, std::size_t aByteCountToCopy);
		void CopyTextureRegion(GPUResource& aSource, std::size_t aSourceOffset, SubresourceLayouts& someSubResourceLayouts, std::uint32_t aSubresourceCount, GPUResource& aDestination);

	protected:
		void BindDescriptorHeaps();

		Device& myDevice;
#ifdef TRACY_ENABLE
		TracyD3D12Ctx& myProfilingContext;
#endif

		D3D12_COMMAND_LIST_TYPE myCommandType;
		ComPtr<ID3D12GraphicsCommandList6> myCommandList;
		std::vector<ComPtr<ID3D12CommandAllocator>> myFrameCommandAllocators;

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
			TextureUpload()
			{
				std::memset(&SubresourceLayouts.front(), 0, sizeof(SubresourceLayouts[0]) * FrameContext::MaxTextureSubresourceCount);
			}

			std::shared_ptr<GPUResource> Resource;
			std::unique_ptr<std::uint8_t> BufferData;
			std::uint64_t BufferSize = 0;
			std::uint32_t SubresourceCount = 0;
			SubresourceLayouts SubresourceLayouts;
		};

	public:
		UploadContext(Device& aDevice, CommandQueue& aCommandQueue);
		~UploadContext();

		//void AddUpload(Buffer);
		TextureUpload& AddTextureUpload();
		void ProcessUploads();
		void ResolveUploads();

	private:
		std::unique_ptr<BackendGraphicsBuffer> myBufferUploadHeap;
		std::unique_ptr<BackendGraphicsBuffer> myTextureUploadHeap;

		std::vector<std::shared_ptr<GPUResource>> myBufferUploadsInProgress;
		std::vector<std::shared_ptr<GPUResource>> myTextureUploadsInProgress;

		std::vector<BufferUpload> myBufferUploads;
		std::vector<TextureUpload> myTextureUploads;
	};

	class FrameGraphicsContext final : public FrameContext, public Atrium::FrameGraphicsContext
	{
	public:
		FrameGraphicsContext(Device& aDevice, CommandQueue& aCommandQueue);

		void BeginProfileZone(ProfileContextZone& aZoneScope
#ifdef TRACY_ENABLE
			, const tracy::SourceLocationData& aLocation
#endif
		) override;

		void Reset(const std::uint_least8_t& aFrameInFlight) override;

		void ClearColor(const std::shared_ptr<Atrium::RenderTexture>& aTarget, Color aClearColor) override;
		void ClearDepth(const std::shared_ptr<Atrium::RenderTexture>& aTarget, float aDepth, std::uint8_t aStencil) override;

		void DisableScissorRect() override;

		void Dispatch(std::uint32_t aGroupCountX, std::uint32_t aGroupCountY, std::uint32_t aGroupCountZ) override;
		void Dispatch1D(std::uint32_t aThreadCountX, std::uint32_t aGroupSizeX) override;
		void Dispatch2D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY) override;
		void Dispatch3D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aThreadCountZ, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY, std::uint32_t aGroupSizeZ) override;

		void Draw(std::uint32_t aVertexCount, std::uint32_t aVertexStartOffset) override;
		void DrawIndexed(std::uint32_t anIndexCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation) override;
		void DrawInstanced(std::uint32_t aVertexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartVertexLocation, std::uint32_t aStartInstanceLocation) override;
		void DrawIndexedInstanced(std::uint32_t anIndexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation, std::uint32_t aStartInstanceLocation) override;

		void SetBlendFactor(Color aBlendFactor) override;
		void SetPipelineState(const std::shared_ptr<Atrium::PipelineState>& aPipelineState) override;
		void SetVertexBuffer(const std::shared_ptr<const Atrium::GraphicsBuffer>& aVertexBuffer, unsigned int aSlot) override;
		void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Atrium::GraphicsBuffer>& aBuffer) override;
		void SetPipelineResource(ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Texture>& aTexture) override;
		void SetPrimitiveTopology(PrimitiveTopology aTopology) override;
		void SetScissorRect(const Rectangle& aRectangle) override;
		void SetStencilRef(std::uint32_t aStencilRef) override;
		void SetRenderTargets(const std::vector<std::shared_ptr<Atrium::RenderTexture>>& someTargets, const std::shared_ptr<Atrium::RenderTexture>& aDepthTarget) override;
		void SetViewportAndScissorRect(const Size& aScreenSize) override;
		void SetViewport(const RectangleF& aRectangle) override;

	private:
		inline std::uint32_t GetGroupCount(std::uint32_t threadCount, std::uint32_t groupSize) { return (threadCount + groupSize - 1) / groupSize; }
		void FlushPipelineConstantBuffers();
		void FlushPipelineTextures();
		void FlushPipelineResources();

		PipelineState* myCurrentPipelineState;

		std::map<std::vector<std::shared_ptr<Atrium::GraphicsBuffer>>, DescriptorHeapHandle> myBufferHeapHandles;
		std::map<std::vector<std::shared_ptr<Atrium::Texture>>, DescriptorHeapHandle> myTextureHeapHandles;

		std::map<std::uint32_t, std::vector<std::shared_ptr<Atrium::GraphicsBuffer>>> myPendingBufferResources;
		std::map<std::uint32_t, std::vector<std::shared_ptr<Atrium::Texture>>> myPendingTextureResources;
	};
}