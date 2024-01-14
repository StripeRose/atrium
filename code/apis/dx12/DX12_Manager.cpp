#include "DX12_Manager.hpp"

#include "DX12_CommandBuffer.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_DDS.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_Shader.hpp"

#include "Common_Debug.hpp"

#ifdef _DEBUG
#include <dxgi1_6.h>
#include <dxgidebug.h>
#endif

#include <algorithm>

namespace RoseGold::DirectX12
{
	Manager::Manager()
		: myComputeQueueFrameEndFence(0)
		, myCopyQueueFrameEndFence(0)
		, myGraphicsQueueFrameEndFence(0)
		, myFrameIndex(static_cast<std::uint64_t>(-1))
	{
		Debug::Log("DX12 start");

		myDevice.reset(new Device());

		myCommandQueueManager.reset(new CommandQueueManager(myDevice->GetDevice()));

		myFrameGraphicsContext.reset(new FrameGraphicsContext(*myDevice));
		myUploadContext.reset(new UploadContext(*myDevice));

		SetupRootSignature();
	}

	Manager::~Manager()
	{
		Debug::Log("DX12 stop");

		myDefaultRootSignature.reset();

		myUploadContext.reset();
		myFrameGraphicsContext.reset();

		myCommandQueueManager.reset();

		myDrawSurfaceSwapChain.clear();
		myDevice.reset();

		ReportUnreleasedObjects();
	}

	std::shared_ptr<Core::Graphics::RenderTexture> Manager::CreateRenderTextureForWindow(Core::Platform::Window& aWindow)
	{
		const std::scoped_lock lock(mySwapChainMutex);
		std::shared_ptr<SwapChain>& swapChain = myDrawSurfaceSwapChain[&aWindow];
		swapChain.reset(new SwapChain(*myDevice, myCommandQueueManager->GetGraphicsQueue(), aWindow));

		aWindow.Closed.Connect(this, [&](Core::Platform::Window& aWindow) {
			myDrawSurfaceSwapChain.at(&aWindow)->Invalidate();
			myDrawSurfaceSwapChain.erase(&aWindow);
			});

		return swapChain;
	}

	std::shared_ptr<Core::Graphics::GraphicsBuffer> Manager::CreateGraphicsBuffer(Core::Graphics::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		switch (aTarget)
		{
		case Core::Graphics::GraphicsBuffer::Target::Vertex:
			return std::shared_ptr<Core::Graphics::GraphicsBuffer>(new VertexBuffer(*myDevice, aCount, aStride));
		case Core::Graphics::GraphicsBuffer::Target::Index:
			return std::shared_ptr<Core::Graphics::GraphicsBuffer>(new IndexBuffer(*myDevice, aCount));
		default:
			return nullptr;
		}
	}

	std::shared_ptr<Core::Graphics::PipelineState> Manager::CreateOrGetPipelineState(const Core::Graphics::PipelineStateDescription& aPipelineState)
	{
		return DirectX12::PipelineState::CreateFrom(*myDevice->GetDevice().Get(), myDefaultRootSignature, aPipelineState);
	}

	std::shared_ptr<Core::Graphics::Shader> Manager::CreateShader(const std::filesystem::path& aSource, Core::Graphics::Shader::Type aType, const char* anEntryPoint)
	{
		switch (aType)
		{
		case Shader::Type::Vertex:
			return Shader::CreateFromSource(aSource, anEntryPoint, "vs_5_1");

		case Shader::Type::Pixel:
			return Shader::CreateFromSource(aSource, anEntryPoint, "ps_5_1");

		default:
			return nullptr;
		}
	}

	void Manager::ExecuteCommandBuffer(const Core::Graphics::CommandBuffer& aCommandBuffer)
	{
		ResolvedCommandBuffer resolvedBuffer(*myFrameGraphicsContext);
		resolvedBuffer.Resolve(aCommandBuffer);
	}

	void Manager::ExecuteTask(const Core::Graphics::GraphicsTask& aGraphicsTask)
	{
		std::vector<const Core::Graphics::GraphicsTask*> resolvedTasks;
		aGraphicsTask.ResolveWorkTasks(resolvedTasks);
		for (const auto& task : resolvedTasks)
		{
			const std::vector<std::shared_ptr<Core::Graphics::CommandBuffer>>& taskWork = task->GetWork();
			for (const std::shared_ptr<Core::Graphics::CommandBuffer>& commandBuffer : taskWork)
				ExecuteCommandBuffer(*commandBuffer);
		}
	}

	std::shared_ptr<SwapChain> Manager::GetSwapChain(Core::Platform::Window& aWindow)
	{
		const std::scoped_lock lock(mySwapChainMutex);
		auto it = myDrawSurfaceSwapChain.find(&aWindow);
		return (it != myDrawSurfaceSwapChain.end()) ? it->second : nullptr;
	}

	std::vector<std::shared_ptr<SwapChain>> Manager::GetSwapChains()
	{
		const std::scoped_lock lock(mySwapChainMutex);
		std::vector<std::shared_ptr<SwapChain>> swapChains;
		for (auto& swapChain : myDrawSurfaceSwapChain)
			swapChains.push_back(swapChain.second);
		return swapChains;
	}

	std::shared_ptr<Core::Graphics::Texture> Manager::LoadTexture(const std::filesystem::path& aPath)
	{
		const std::filesystem::path extension = aPath.extension();
		if (extension == ".dds")
			return Texture2D_DDS::LoadFromFile(aPath, *myDevice, *myUploadContext);
		else
			return nullptr;
	}

	void Manager::MarkFrameStart()
	{
		myFrameIndex += 1;

		myCommandQueueManager->GetComputeQueue().WaitForFenceCPUBlocking(myComputeQueueFrameEndFence);
		myCommandQueueManager->GetCopyQueue().WaitForFenceCPUBlocking(myCopyQueueFrameEndFence);
		myCommandQueueManager->GetGraphicsQueue().WaitForFenceCPUBlocking(myGraphicsQueueFrameEndFence);

		myUploadContext->ResolveUploads();
		myUploadContext->Reset();
		myFrameGraphicsContext->Reset();

		const std::scoped_lock lock(mySwapChainMutex);
		for (auto& swapChain : myDrawSurfaceSwapChain)
			swapChain.second->UpdateResolution();

		myDevice->MarkFrameStart();
	}

	void Manager::MarkFrameEnd()
	{
		myUploadContext->ProcessUploads();
		myCommandQueueManager->GetCopyQueue().ExecuteCommandList(
			myUploadContext->GetCommandList()
		);

		// Record used swapchains and make them ready to present.
		std::vector<std::shared_ptr<SwapChain>> frameSwapChains;
		for (const auto& swapChainIterator : myDrawSurfaceSwapChain)
		{
			if (swapChainIterator.second->GetGPUResource().GetUsageState() == D3D12_RESOURCE_STATE_PRESENT)
				continue; // Swapchain hasn't been drawn to, skip it.

			frameSwapChains.push_back(swapChainIterator.second);
			myFrameGraphicsContext->AddBarrier(swapChainIterator.second->GetGPUResource(), D3D12_RESOURCE_STATE_PRESENT);
		}
		myFrameGraphicsContext->FlushBarriers();

		// Submit the frame's work.
		CommandQueue& graphicsQueue = myCommandQueueManager->GetGraphicsQueue();
		graphicsQueue.ExecuteCommandList(
			myFrameGraphicsContext->GetCommandList()
		);

		for (const std::shared_ptr<SwapChain>& swapChain : frameSwapChains)
			swapChain->Present();

		myComputeQueueFrameEndFence = myCommandQueueManager->GetComputeQueue().InsertSignal();
		myCopyQueueFrameEndFence = myCommandQueueManager->GetCopyQueue().InsertSignal();
		myGraphicsQueueFrameEndFence = myCommandQueueManager->GetGraphicsQueue().InsertSignal();
	}

	void Manager::SetupRootSignature()
	{
		RootSignatureCreator signature;

		signature.SetVisibility(D3D12_SHADER_VISIBILITY_VERTEX);
		{
			signature.AddDescriptorTable()
				.AddCBVRange(1, 0, RootParameterUpdateFrequency::PerObject) // Model, View, Projection data.
				;
		}

		signature.SetVisibility(D3D12_SHADER_VISIBILITY_PIXEL);
		{
			signature.AddDescriptorTable()
				.AddSRVRange(4, 0, RootParameterUpdateFrequency::PerMaterial)
				;

			signature.AddSampler(0) // Wrapping Point
				.Filter(D3D12_FILTER_MIN_MAG_MIP_POINT)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_WRAP)
				;
			signature.AddSampler(1) // Wrapping Linear
				.Filter(D3D12_FILTER_MIN_MAG_MIP_LINEAR)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_WRAP)
				;
			signature.AddSampler(2) // Wrapping Anisotropic
				.Filter(D3D12_FILTER_ANISOTROPIC)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_WRAP)
				;
		}

		myDefaultRootSignature = signature.Finalize(myDevice->GetDevice().Get());
	}

	void Manager::ReportUnreleasedObjects()
	{
#if _DEBUG
		ComPtr<IDXGIDebug1> debugInterface;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debugInterface.GetAddressOf()))))
		{
			//debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
#endif
	}
}
