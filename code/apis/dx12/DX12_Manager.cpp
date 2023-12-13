#include "DX12_Manager.hpp"

#include "DX12_CommandBuffer.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"

#include "Common_Debug.hpp"

#ifdef _DEBUG
#include <dxgi1_6.h>
#include <dxgidebug.h>
#endif

namespace RoseGold::DirectX12
{
	Manager::Manager()
	{
		Debug::Log("DX12 start");
		myDevice.reset(new Device());

		AssertSuccess(
			myDevice->GetDevice()->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(myCommandAllocator.ReleaseAndGetAddressOf())
			)
		);
	}

	Manager::~Manager()
	{
		Debug::Log("DX12 stop");

		myCommandAllocator->Reset();
		myCommandAllocator.Reset();
		myDevice.reset();
		ReportUnreleasedObjects();
	}

	std::shared_ptr<Core::Graphics::RenderTexture> Manager::CreateRenderTextureForWindow(Core::Platform::Window& aWindow)
	{
		return myDevice->CreateRenderTextureForWindow(aWindow);
	}

	void Manager::ExecuteCommandBuffer(const Core::Graphics::CommandBuffer& aCommandBuffer)
	{
		ResolvedCommandBuffer resolvedBuffer(*myDevice, myCommandAllocator.Get());
		resolvedBuffer.Resolve(aCommandBuffer);
		myDevice->GetCommandQueueManager().GetGraphicsQueue().ExecuteCommandList(
			resolvedBuffer.GetCommandList()
		);
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
		myDevice->UpdateSwapchainResolutions();
		myCommandAllocator->Reset();
	}

	void Manager::MarkFrameEnd()
	{
		std::vector<std::shared_ptr<SwapChain>> swapChains = myDevice->GetSwapChains();
		for (std::shared_ptr<SwapChain> swapChain : swapChains)
			swapChain->Present();
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
