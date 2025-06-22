#include "Core_Diagnostics.hpp"

#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Instancer.hpp"
#include "DX12_ImGuiContext.hpp"
#include "DX12_Manager.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_Shader.hpp"

#ifdef IS_DEBUG_BUILD
#include <dxgi1_6.h>
#include <dxgidebug.h>
#endif

#include <algorithm>

namespace Atrium::DirectX12
{
	std::unique_ptr<Core::GraphicsAPI> CreateDX12Manager()
	{
		return std::make_unique<DirectX12API>();
	}

	std::size_t DirectX12API::GetFramesInFlightAmount()
	{
		return DX12_FRAMES_IN_FLIGHT;
	}

	DirectX12API::DirectX12API()
		: myFrameIndex(static_cast<std::uint64_t>(-1))
		, myFrameInFlight(0)
	{
		ZoneScoped;

		myFrameEndFences.resize(DX12_FRAMES_IN_FLIGHT);
		for (auto& frameEndFence : myFrameEndFences)
		{
			frameEndFence.ComputeQueue = 0;
			frameEndFence.CopyQueue = 0;
			frameEndFence.GraphicsQueue = 0;
		}

		Debug::Log("DX12 start");

		myDevice.reset(new Device());

		myCommandQueueManager.reset(new CommandQueueManager(myDevice->GetDevice()));

		myPresentPrepareContext.reset(new FrameGraphicsContext(*myDevice, myCommandQueueManager->GetGraphicsQueue()));
		myUploadContext.reset(new UploadContext(*myDevice, myCommandQueueManager->GetCopyQueue()));

		myResourceManager.reset(new DirectX12::ResourceManager(*this));
	}

	DirectX12API::~DirectX12API()
	{
		ZoneScoped;

		Debug::Log("DX12 stop");

		myResourceManager.reset();

		myUploadContext.reset();
		myPresentPrepareContext.reset();
		myFrameGraphicsContexts.clear();

		myCommandQueueManager.reset();

		myDevice.reset();

		ReportUnreleasedObjects();
	}

	std::shared_ptr<Core::FrameGraphicsContext> DirectX12API::CreateFrameGraphicsContext()
	{
		return myFrameGraphicsContexts.emplace_back(
			new FrameGraphicsContext(*myDevice, myCommandQueueManager->GetGraphicsQueue())
		);
	}

	std::unique_ptr<Core::ImGuiContext> DirectX12API::CreateImGuiContext(const std::shared_ptr<Core::RenderTexture>& aRenderTarget)
	{
		return std::unique_ptr<Core::ImGuiContext>(
			new ImGuiContext(*this, *myPresentPrepareContext, aRenderTarget)
		);
	}

	std::uint_least64_t DirectX12API::GetCurrentFrameIndex() const
	{
		return myFrameIndex;
	}

	void DirectX12API::MarkFrameStart()
	{
		ZoneScoped;

		myFrameIndex += 1;
		myFrameInFlight = myFrameIndex % DX12_FRAMES_IN_FLIGHT;

		{
			ZoneScopedN("Waiting for previous frame");

			myCommandQueueManager->GetComputeQueue().WaitForFenceCPUBlocking(myFrameEndFences[myFrameInFlight].ComputeQueue);
			myCommandQueueManager->GetCopyQueue().WaitForFenceCPUBlocking(myFrameEndFences[myFrameInFlight].CopyQueue);
			myCommandQueueManager->GetGraphicsQueue().WaitForFenceCPUBlocking(myFrameEndFences[myFrameInFlight].GraphicsQueue);
		}

		myDevice->GetDescriptorHeapManager().GetFrameHeap(myFrameInFlight).Reset();

		myUploadContext->ResolveUploads();
		myUploadContext->Reset(myFrameInFlight);
		for (auto& contextIterator : myFrameGraphicsContexts)
			contextIterator->Reset(myFrameInFlight);
		myPresentPrepareContext->Reset(myFrameInFlight);

		myResourceManager->MarkFrameStart();
	}

	void DirectX12API::MarkFrameEnd()
	{
		ZoneScoped;

		{
			ZoneScopedN("Process uploads");
			myUploadContext->ProcessUploads();
			myFrameEndFences[myFrameInFlight].CopyQueue = myCommandQueueManager->GetCopyQueue().ExecuteCommandList(myUploadContext->GetCommandList());
		}

		// Submit the frame's work.
		{
			ZoneScopedN("Submit graphic commands");

			std::vector<ComPtr<ID3D12CommandList>> commandLists;
			commandLists.reserve(myFrameGraphicsContexts.size());
			for (const auto& context : myFrameGraphicsContexts)
				commandLists.emplace_back(context->GetCommandList());

			myFrameEndFences[myFrameInFlight].GraphicsQueue = myCommandQueueManager->GetGraphicsQueue().ExecuteCommandLists(commandLists);
		}

		// Record used swapchains and make them ready to present.
		std::vector<std::shared_ptr<SwapChain>> frameSwapChains;
		{
			ZoneScopedN("Set up used swapchains for presenting");
			for (const auto& swapChain : myResourceManager->GetSwapChains())
			{
				if (swapChain->GetGPUResource()->GetUsageState() == D3D12_RESOURCE_STATE_PRESENT)
					continue; // Swapchain hasn't been drawn to, skip it.

				frameSwapChains.push_back(swapChain);
				myPresentPrepareContext->AddBarrier(*swapChain->GetGPUResource(), D3D12_RESOURCE_STATE_PRESENT);
			}
			
			myPresentPrepareContext->FlushBarriers();

			myFrameEndFences[myFrameInFlight].GraphicsQueue = myCommandQueueManager->GetGraphicsQueue().ExecuteCommandList(myPresentPrepareContext->GetCommandList());
		}

		{
			ZoneScopedN("Present");
			for (const std::shared_ptr<SwapChain>& swapChain : frameSwapChains)
				swapChain->Present();
		}

		TracyD3D12Collect(myCommandQueueManager->GetComputeQueue().GetProfilingContext());
		TracyD3D12Collect(myCommandQueueManager->GetCopyQueue().GetProfilingContext());
		TracyD3D12Collect(myCommandQueueManager->GetGraphicsQueue().GetProfilingContext());

		FrameMark;
		TracyD3D12NewFrame(myCommandQueueManager->GetComputeQueue().GetProfilingContext());
		TracyD3D12NewFrame(myCommandQueueManager->GetCopyQueue().GetProfilingContext());
		TracyD3D12NewFrame(myCommandQueueManager->GetGraphicsQueue().GetProfilingContext());
	}

	void DirectX12API::ReportUnreleasedObjects()
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
