#include "DX12_Manager.hpp"

#include "DX12_CommandBuffer.hpp"
#include "DX12_ComPtr.hpp"
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
	{
		Debug::Log("DX12 start");

		myDevice.reset(new Device());
		myFrameGraphicsContext.reset(new FrameGraphicsContext(*myDevice));
	}

	Manager::~Manager()
	{
		Debug::Log("DX12 stop");

		myFrameGraphicsContext.reset();
		myDevice.reset();
		ReportUnreleasedObjects();
	}

	std::shared_ptr<Core::Graphics::RenderTexture> Manager::CreateRenderTextureForWindow(Core::Platform::Window& aWindow)
	{
		return myDevice->CreateRenderTextureForWindow(aWindow);
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

	std::shared_ptr<Core::Graphics::CachedPipelineState> Manager::CreateOrGetPipelineState(const Core::Graphics::PipelineState& aPipelineState)
	{
		return myDevice->GetPipeline().CreateOrGetState(aPipelineState);
	}

	std::shared_ptr<Core::Graphics::Shader> Manager::CreateShader(const std::filesystem::path& aSource, Core::Graphics::Shader::Type aType, const char* anEntryPoint)
	{
		switch (aType)
		{
		case Shader::Type::Vertex:
			return Shader::CreateFromSource(aSource, anEntryPoint, "vs_5_0");

		case Shader::Type::Pixel:
			return Shader::CreateFromSource(aSource, anEntryPoint, "ps_5_0");

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

	void Manager::MarkFrameStart()
	{
		myDevice->MarkFrameStart();
	}

	void Manager::MarkFrameEnd()
	{
		// Record used swapchains and make them ready to present.
		std::vector<std::shared_ptr<SwapChain>> frameSwapChains;
		for (const std::shared_ptr<SwapChain>& swapChain : myDevice->GetSwapChains())
		{
			if (swapChain->GetGPUResource().GetUsageState() == D3D12_RESOURCE_STATE_PRESENT)
				continue; // Swapchain hasn't been drawn to, skip it.

			frameSwapChains.push_back(swapChain);
			myFrameGraphicsContext->AddBarrier(swapChain->GetGPUResource(), D3D12_RESOURCE_STATE_PRESENT);
		}
		myFrameGraphicsContext->FlushBarriers();

		// Submit the frame's work.
		myDevice->GetCommandQueueManager().GetGraphicsQueue().ExecuteCommandList(
			myFrameGraphicsContext->GetCommandList()
		);

		for (const std::shared_ptr<SwapChain>& swapChain : frameSwapChains)
			swapChain->Present();

		CommandQueue& queue = myDevice->GetCommandQueueManager().GetGraphicsQueue();
		queue.WaitForFenceCPUBlocking(queue.InsertSignal());

		myFrameGraphicsContext->Reset();
		myDevice->MarkFrameEnd();
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
