#include "DX12_Manager.hpp"

#include "DX12_CommandBuffer.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Mesh.hpp"

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

		AssertSuccess(
			myDevice->GetDevice()->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(myCommandAllocator.ReleaseAndGetAddressOf())
			)
		);

		Mesh::Prepare(myDevice->GetDevice().Get());
	}

	Manager::~Manager()
	{
		Debug::Log("DX12 stop");

		Mesh::Cleanup();

		myCommandAllocator->Reset();
		myCommandAllocator.Reset();
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

	std::shared_ptr<Core::Graphics::Mesh> Manager::CreateMesh()
	{
		return std::shared_ptr<Core::Graphics::Mesh>(new Mesh(*this));
	}

	void Manager::ExecuteCommandBuffer(const Core::Graphics::CommandBuffer& aCommandBuffer)
	{
		ResolvedCommandBuffer resolvedBuffer(*myDevice, myCommandAllocator.Get());
		resolvedBuffer.Resolve(aCommandBuffer);

		for (const auto& target : resolvedBuffer.GetUsedTargets())
		{
			if (std::find(myFrameTargets.begin(), myFrameTargets.end(), target) == myFrameTargets.end())
				myFrameTargets.push_back(target);
		}

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
		myDevice->MarkFrameStart();
		myCommandAllocator->Reset();
	}

	void Manager::MarkFrameEnd()
	{
		for (const std::shared_ptr<Core::Graphics::RenderTexture>& renderTarget : myFrameTargets)
		{
			RenderTarget* target = static_cast<RenderTarget*>(renderTarget.get());
			if (target->IsSwapChain())
			{
				SwapChain* swapChain = static_cast<SwapChain*>(target);
				swapChain->Present();
			}
		}

		CommandQueue& queue = myDevice->GetCommandQueueManager().GetGraphicsQueue();
		queue.WaitForFenceCPUBlocking(queue.InsertSignal());

		myCommandAllocator->Reset();
		myFrameTargets.clear();
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
