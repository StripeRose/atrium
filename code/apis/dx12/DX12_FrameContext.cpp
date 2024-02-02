// Filter "Frame contexts"

#include "stdafx.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_FrameContext.hpp"
#include "DX12_Manager.hpp"
#include "DX12_MemoryAlignment.hpp"
#include "DX12_RenderTexture.hpp"

namespace RoseGold::DirectX12
{
	FrameContext::FrameContext(Device& aDevice, D3D12_COMMAND_LIST_TYPE aCommandType)
		: myDevice(aDevice)
		, myCommandType(aCommandType)
		, myCurrentFrameHeap(nullptr)
		, myResourceBarrierQueue()
	{
		AssertAction(
			aDevice.GetDevice()->CreateCommandAllocator(
				aCommandType,
				IID_PPV_ARGS(myCommandAllocator.ReleaseAndGetAddressOf())
			),
			"Create frame context command allocator."
		);

		ComPtr<ID3D12Device4> device4;
		AssertAction(aDevice.GetDevice().As<ID3D12Device4>(&device4), "Get ID3D12Device4.");
		
		// Create a closed command list.
		AssertAction(
			device4->CreateCommandList1(
				0,
				aCommandType,
				D3D12_COMMAND_LIST_FLAG_NONE,
				IID_PPV_ARGS(myCommandList.ReleaseAndGetAddressOf())
			),
			"Create closed command list."
		);
	}

	void FrameContext::Reset()
	{
		myCommandAllocator->Reset();
		myCommandList->Reset(myCommandAllocator.Get(), nullptr);

		if (myCommandType != D3D12_COMMAND_LIST_TYPE_COPY)
			BindDescriptorHeaps();
	}

	void FrameContext::AddBarrier(GPUResource& aResource, D3D12_RESOURCE_STATES aNewState)
	{
		if (myQueuedBarriers >= ourMaxQueuedBarriers)
			FlushBarriers();

		if (myCommandType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			constexpr D3D12_RESOURCE_STATES VALID_COMPUTE_CONTEXT_STATES = (D3D12_RESOURCE_STATE_UNORDERED_ACCESS | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
				D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_COPY_SOURCE);

			const D3D12_RESOURCE_STATES oldState = aResource.GetUsageState();
			Debug::Assert((oldState & VALID_COMPUTE_CONTEXT_STATES) == oldState, "Current state is valid for compute contexts.");
			Debug::Assert((aNewState & VALID_COMPUTE_CONTEXT_STATES) == aNewState, "Target state is valid for compute contexts.");
		}

		std::optional<D3D12_RESOURCE_BARRIER> barrier = aResource.UpdateUsageState(aNewState);
		if (barrier.has_value())
		{
			myResourceBarrierQueue.at(myQueuedBarriers) = barrier.value();
			myQueuedBarriers++;
		}
	}

	void FrameContext::FlushBarriers()
	{
		if (myQueuedBarriers == 0)
			return;

		myCommandList->ResourceBarrier(static_cast<UINT>(myQueuedBarriers), myResourceBarrierQueue.data());
		myQueuedBarriers = 0;
	}

	void FrameContext::CopyResource(const GPUResource& aSource, GPUResource& aDestination)
	{
		myCommandList->CopyResource(aDestination.GetResource().Get(), aSource.GetResource().Get());
	}

	void FrameContext::CopyBufferRegion(const GPUResource& aSource, std::size_t aSourceOffset, GPUResource& aDestination, std::size_t aDestinationOffset, std::size_t aByteCountToCopy)
	{
		myCommandList->CopyBufferRegion(
			aDestination.GetResource().Get(),
			static_cast<UINT64>(aDestinationOffset),
			aSource.GetResource().Get(),
			static_cast<UINT64>(aSourceOffset),
			static_cast<UINT64>(aByteCountToCopy)
		);
	}

	void FrameContext::CopyTextureRegion(GPUResource& aSource, std::size_t aSourceOffset, SubresourceLayouts& someSubResourceLayouts, std::uint32_t aSubresourceCount, GPUResource& aDestination)
	{
		for (std::uint32_t subResourceIndex = 0; subResourceIndex < aSubresourceCount; subResourceIndex++)
		{
			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = aDestination.GetResource().Get();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = subResourceIndex;

			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = aSource.GetResource().Get();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint = someSubResourceLayouts[subResourceIndex];
			sourceLocation.PlacedFootprint.Offset += aSourceOffset;

			myCommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
		}
	}

	void FrameContext::BindDescriptorHeaps()
	{
		DescriptorHeapManager& heapManager = myDevice.GetDescriptorHeapManager();

		myCurrentFrameHeap = &heapManager.GetFrameHeap();
		myCurrentFrameHeap->Reset();

		ID3D12DescriptorHeap* heapsToBind[1];
		heapsToBind[0] = myCurrentFrameHeap->GetHeap().Get();
		//heapsToBind[1] = heapManager.GetFrameSamplerHeap().GetHeap().Get();

		myCommandList->SetDescriptorHeaps(1, heapsToBind);
	}

	UploadContext::UploadContext(Device& aDevice)
		: FrameContext(aDevice, D3D12_COMMAND_LIST_TYPE_COPY)
	{
		myBufferUploadHeap.reset(new UploadBuffer(aDevice, 10 * 1024 * 1024));
		myTextureUploadHeap.reset(new UploadBuffer(aDevice, 40 * 1024 * 1024));
	}

	UploadContext::TextureUpload& UploadContext::AddTextureUpload()
	{
		return myTextureUploads.emplace_back();
	}

	void UploadContext::ProcessUploads()
	{
		const uint32_t numBufferUploads = static_cast<uint32_t>(myBufferUploads.size());
		const uint32_t numTextureUploads = static_cast<uint32_t>(myTextureUploads.size());
		uint32_t numBuffersProcessed = 0;
		uint32_t numTexturesProcessed = 0;
		size_t bufferUploadHeapOffset = 0;
		size_t textureUploadHeapOffset = 0;

		for (numBuffersProcessed; numBuffersProcessed < numBufferUploads; numBuffersProcessed++)
		{
			BufferUpload& currentUpload = myBufferUploads[numBuffersProcessed];

			if ((bufferUploadHeapOffset + currentUpload.BufferSize) > myBufferUploadHeap->GetStride())
				break;

			myBufferUploadHeap->SetData(bufferUploadHeapOffset, currentUpload.BufferData.get(), static_cast<std::uint32_t>(currentUpload.BufferSize));
			CopyBufferRegion(*currentUpload.Resource, 0, *myBufferUploadHeap, bufferUploadHeapOffset, currentUpload.BufferSize);

			bufferUploadHeapOffset += currentUpload.BufferSize;
			myBufferUploadsInProgress.push_back(currentUpload.Resource);
		}

		for (numTexturesProcessed; numTexturesProcessed < numTextureUploads; numTexturesProcessed++)
		{
			TextureUpload& currentUpload = myTextureUploads[numTexturesProcessed];

			if ((textureUploadHeapOffset + currentUpload.BufferSize) > myTextureUploadHeap->GetStride())
				break;

			myTextureUploadHeap->SetData(textureUploadHeapOffset, currentUpload.BufferData.get(), static_cast<std::uint32_t>(currentUpload.BufferSize));
			CopyTextureRegion(*myTextureUploadHeap, textureUploadHeapOffset, currentUpload.SubresourceLayouts, currentUpload.SubresourceCount, *currentUpload.Resource);

			textureUploadHeapOffset += currentUpload.BufferSize;
			textureUploadHeapOffset = Align<std::size_t>(textureUploadHeapOffset, 512);

			myTextureUploadsInProgress.push_back(currentUpload.Resource);
		}

		if (numBuffersProcessed > 0)
			myBufferUploads.erase(myBufferUploads.begin(), myBufferUploads.begin() + numBuffersProcessed);

		if (numTexturesProcessed > 0)
			myTextureUploads.erase(myTextureUploads.begin(), myTextureUploads.begin() + numTexturesProcessed);
	}

	void UploadContext::ResolveUploads()
	{
		for (auto& bufferUploadInProgress : myBufferUploadsInProgress)
			bufferUploadInProgress->myIsReady = true;

		for (auto& textureUploadInProgress : myTextureUploadsInProgress)
			textureUploadInProgress->myIsReady = true;

		myBufferUploadsInProgress.clear();
		myTextureUploadsInProgress.clear();
	}

	FrameGraphicsContext::FrameGraphicsContext(Device& aDevice)
		: DirectX12::FrameContext(aDevice, D3D12_COMMAND_LIST_TYPE_DIRECT)
		, myCurrentPipelineState(nullptr)
	{ }

	void FrameGraphicsContext::Reset()
	{
		DirectX12::FrameContext::Reset();
	}

	void FrameGraphicsContext::ClearColor(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, Color aClearColor)
	{
		Debug::Assert(!!aTarget, "ClearColor() requires a target.");

		RenderTarget& target = static_cast<RenderTarget&>(*aTarget);
		AddBarrier(target.GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		FlushBarriers();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = target.GetColorView()->GetCPUHandle();

		if (!Debug::Verify(handle.ptr != NULL, "Invalid target."))
			return;

		float color[4] = {
			aClearColor.R,
			aClearColor.G,
			aClearColor.B,
			aClearColor.A
		};
		myCommandList->ClearRenderTargetView(
			handle,
			color,
			0,
			nullptr
		);
	}

	void FrameGraphicsContext::ClearDepth(const std::shared_ptr<Core::Graphics::RenderTexture>& aTarget, float aDepth, std::uint8_t aStencil)
	{
		Debug::Assert(!!aTarget, "ClearDepth() requires a target.");

		RenderTarget& target = static_cast<RenderTarget&>(*aTarget);
		AddBarrier(target.GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		FlushBarriers();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = target.GetDepthStencilView()->GetCPUHandle();

		if (!Debug::Verify(handle.ptr != NULL, "Invalid target."))
			return;

		myCommandList->ClearDepthStencilView(
			handle,
			D3D12_CLEAR_FLAG_DEPTH,
			aDepth,
			aStencil, 0, nullptr
		);
	}

	void FrameGraphicsContext::DisableScissorRect()
	{
		myCommandList->RSSetScissorRects(0, nullptr);
	}

	void FrameGraphicsContext::Dispatch(std::uint32_t aGroupCountX, std::uint32_t aGroupCountY, std::uint32_t aGroupCountZ)
	{
		myCommandList->Dispatch(aGroupCountX, aGroupCountY, aGroupCountZ);
	}

	void FrameGraphicsContext::Dispatch1D(std::uint32_t aThreadCountX, std::uint32_t aGroupSizeX)
	{
		Dispatch(GetGroupCount(aThreadCountX, aGroupSizeX), 1, 1);
	}

	void FrameGraphicsContext::Dispatch2D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY)
	{
		Dispatch(GetGroupCount(aThreadCountX, aGroupSizeX), GetGroupCount(aThreadCountY, aGroupSizeY), 1);
	}

	void FrameGraphicsContext::Dispatch3D(std::uint32_t aThreadCountX, std::uint32_t aThreadCountY, std::uint32_t aThreadCountZ, std::uint32_t aGroupSizeX, std::uint32_t aGroupSizeY, std::uint32_t aGroupSizeZ)
	{
		Dispatch(GetGroupCount(aThreadCountX, aGroupSizeX), GetGroupCount(aThreadCountY, aGroupSizeY), GetGroupCount(aThreadCountZ, aGroupSizeZ));
	}

	void FrameGraphicsContext::Draw(std::uint32_t aVertexCount, std::uint32_t aVertexStartOffset)
	{
		DrawInstanced(aVertexCount, 1, aVertexStartOffset, 0);
	}

	void FrameGraphicsContext::DrawIndexed(std::uint32_t anIndexCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation)
	{
		DrawIndexedInstanced(anIndexCount, 1, aStartIndexLocation, aBaseVertexLocation, 0);
	}

	void FrameGraphicsContext::DrawInstanced(std::uint32_t aVertexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartVertexLocation, std::uint32_t aStartInstanceLocation)
	{
		myCommandList->DrawInstanced(aVertexCountPerInstance, anInstanceCount, aStartVertexLocation, aStartInstanceLocation);
	}

	void FrameGraphicsContext::DrawIndexedInstanced(std::uint32_t anIndexCountPerInstance, std::uint32_t anInstanceCount, std::uint32_t aStartIndexLocation, std::uint32_t aBaseVertexLocation, std::uint32_t aStartInstanceLocation)
	{
		myCommandList->DrawIndexedInstanced(anIndexCountPerInstance, anInstanceCount, aStartIndexLocation, aBaseVertexLocation, aStartInstanceLocation);
	}

	void FrameGraphicsContext::SetBlendFactor(Color aBlendFactor)
	{
		float color[4] = { aBlendFactor.R, aBlendFactor.G, aBlendFactor.B, aBlendFactor.A };
		myCommandList->OMSetBlendFactor(color);
	}

	void FrameGraphicsContext::SetScissorRect(const Math::RectangleT<int>& aRectangle)
	{
		const Math::Vector2T<int> halfSize = (aRectangle.Size / 2);

		D3D12_RECT rect;
		rect.left = aRectangle.Center.X - halfSize.X;
		rect.right = aRectangle.Center.X + halfSize.X;
		rect.top = aRectangle.Center.Y - halfSize.Y;
		rect.bottom = aRectangle.Center.Y + halfSize.Y;

		myCommandList->RSSetScissorRects(1, &rect);
	}

	void FrameGraphicsContext::SetPrimitiveTopology(Core::Graphics::PrimitiveTopology aTopology)
	{
		D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

		switch (aTopology)
		{
		case Core::Graphics::PrimitiveTopology::PointList:
			topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case Core::Graphics::PrimitiveTopology::LineList:
			topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case Core::Graphics::PrimitiveTopology::LineStrip:
			topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		case Core::Graphics::PrimitiveTopology::TriangleList:
			topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case Core::Graphics::PrimitiveTopology::TriangleStrip:
			topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		}

		myCommandList->IASetPrimitiveTopology(topology);
	}

	void FrameGraphicsContext::SetPipelineState(const std::shared_ptr<Core::Graphics::PipelineState>& aPipelineState)
	{
		Debug::Assert(!!aPipelineState, "SetPipelineState() requires pipeline state to be non-null.");

		myCurrentPipelineState = static_cast<DirectX12::PipelineState*>(aPipelineState.get());
		myCommandList->SetPipelineState(myCurrentPipelineState->GetPipelineStateObject().Get());
		myCommandList->SetGraphicsRootSignature(myCurrentPipelineState->GetRootSignature()->GetRootSignatureObject().Get());
	}

	void FrameGraphicsContext::SetVertexBuffer(const std::shared_ptr<const Core::Graphics::GraphicsBuffer>& aVertexBuffer)
	{
		const VertexBuffer& vertexBuffer = static_cast<const VertexBuffer&>(*aVertexBuffer);
		myCommandList->IASetVertexBuffers(0, 1, &vertexBuffer.GetBufferView());
	}

	void FrameGraphicsContext::SetPipelineResource(Core::Graphics::ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Core::Graphics::GraphicsBuffer>& aBuffer)
	{
		const std::optional<RootParameterMapping::ParameterInfo> parameterInfo = myCurrentPipelineState->GetRootSignature()->GetParameterInfo(
			anUpdateFrequency,
			RootParameterMapping::RegisterType::ConstantBuffer,
			aRegisterIndex
		);

		if (!parameterInfo.has_value())
		{
			Debug::LogError("Root parameter missing for register c%i, space%i", aRegisterIndex, static_cast<unsigned int>(anUpdateFrequency));
			return;
		}

		if (parameterInfo.value().Count != 1)
		{
			Debug::LogError("Root parameter %i requires more than 1 resources.", parameterInfo.value().RootParameterIndex);
			return;
		}

		ConstantBuffer& cpuBuffer = static_cast<ConstantBuffer&>(*aBuffer);
		AddBarrier(cpuBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		
		RenderPassDescriptorHeap& renderPassHeap = myDevice.GetDescriptorHeapManager().GetFrameHeap();
		std::shared_ptr<DescriptorHeapHandle> gpuBuffer = renderPassHeap.GetHeapHandleBlock(1);

		myDevice.GetDevice()->CopyDescriptorsSimple(
			1,
			gpuBuffer->GetCPUHandle(),
			cpuBuffer.GetViewHandle().GetCPUHandle(),
			renderPassHeap.GetHeapType()
		);

		myCommandList->SetGraphicsRootDescriptorTable(parameterInfo.value().RootParameterIndex, gpuBuffer->GetGPUHandle());
	}

	void FrameGraphicsContext::SetPipelineResource(Core::Graphics::ResourceUpdateFrequency anUpdateFrequency, std::uint32_t aRegisterIndex, const std::shared_ptr<Core::Graphics::Texture>& aTexture)
	{
		Debug::Assert(!!aTexture, "Has a valid texture.");

		const std::optional<RootParameterMapping::ParameterInfo> parameterInfo = myCurrentPipelineState->GetRootSignature()->GetParameterInfo(
			anUpdateFrequency,
			RootParameterMapping::RegisterType::Texture,
			aRegisterIndex
		);

		if (!parameterInfo.has_value())
		{
			Debug::LogError("Root parameter missing for register t%i, space%i", aRegisterIndex, static_cast<unsigned int>(anUpdateFrequency));
			return;
		}

		RenderPassDescriptorHeap& renderPassHeap = myDevice.GetDescriptorHeapManager().GetFrameHeap();
		std::shared_ptr<DescriptorHeapHandle> gpuBuffer = renderPassHeap.GetHeapHandleBlock(parameterInfo.value().Count);

		Texture2D_DDS* dds = static_cast<Texture2D_DDS*>(aTexture.get());
		myDevice.GetDevice()->CopyDescriptorsSimple(
			1,
			gpuBuffer->GetCPUHandle(),
			dds->GetSRVHandle()->GetCPUHandle(),
			renderPassHeap.GetHeapType()
		);

		// Setup null descriptors for remaining slots
		D3D12_SHADER_RESOURCE_VIEW_DESC nullDesc = { };
		nullDesc.Format = dds->GetDXGIFormat();
		nullDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		nullDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		nullDesc.Texture2D.MipLevels = 1;
		nullDesc.Texture2D.MostDetailedMip = 0;
		nullDesc.Texture2D.PlaneSlice = 0;
		nullDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (unsigned int i = 1; i < parameterInfo.value().Count; ++i)
		{
			myDevice.GetDevice()->CreateShaderResourceView(
				nullptr,
				&nullDesc,
				gpuBuffer->GetCPUHandle(i)
				);
		}

		myCommandList->SetGraphicsRootDescriptorTable(parameterInfo.value().RootParameterIndex, gpuBuffer->GetGPUHandle());
	}

	void FrameGraphicsContext::SetStencilRef(std::uint32_t aStencilRef)
	{
		myCommandList->OMSetStencilRef(aStencilRef);
	}

	void FrameGraphicsContext::SetRenderTargets(const std::vector<std::shared_ptr<Core::Graphics::RenderTexture>>& someTargets, const std::shared_ptr<Core::Graphics::RenderTexture>& aDepthTarget)
	{
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> colorHandles;
		colorHandles.fill(D3D12_CPU_DESCRIPTOR_HANDLE { 0 });

		for (std::size_t i = 0; i < someTargets.size(); ++i)
		{
			RenderTarget* dxRenderTarget = static_cast<RenderTarget*>(someTargets.at(i).get());
			AddBarrier(dxRenderTarget->GetGPUResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			colorHandles.at(i) = dxRenderTarget->GetColorView()->GetCPUHandle();
		}

		RenderTarget* dxDepthTarget = static_cast<RenderTarget*>(aDepthTarget.get());
		AddBarrier(dxDepthTarget->GetDepthGPUResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
		const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = aDepthTarget ? dxDepthTarget->GetDepthStencilView()->GetCPUHandle() : D3D12_CPU_DESCRIPTOR_HANDLE();

		FlushBarriers();

		myCommandList->OMSetRenderTargets(
			static_cast<UINT>(someTargets.size()),
			colorHandles.data(),
			false,
			&depthStencilHandle);
	}

	void FrameGraphicsContext::SetViewportAndScissorRect(const Size& aScreenSize)
	{
		D3D12_VIEWPORT viewport;
		viewport.Width = static_cast<float>(aScreenSize.X);
		viewport.Height = static_cast<float>(aScreenSize.Y);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		D3D12_RECT scissor;
		scissor.top = 0;
		scissor.left = 0;
		scissor.bottom = aScreenSize.Y;
		scissor.right = aScreenSize.X;

		myCommandList->RSSetViewports(1, &viewport);
		myCommandList->RSSetScissorRects(1, &scissor);
	}

	void FrameGraphicsContext::SetViewport(const Math::Rectangle& aRectangle)
	{
		const Math::Vector2 topLeft = aRectangle.Center - (aRectangle.Size * 0.5f);

		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = topLeft.X;
		viewport.TopLeftY = topLeft.Y;
		viewport.Width = aRectangle.Size.X;
		viewport.Height = aRectangle.Size.Y;
		viewport.MinDepth = 0.1f;
		viewport.MaxDepth = 1.0f;

		myCommandList->RSSetViewports(1, &viewport);
	}
}
