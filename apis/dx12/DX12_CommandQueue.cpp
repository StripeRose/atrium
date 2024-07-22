// Filter "Commands"

#include "DX12_CommandQueue.hpp"
#include "DX12_Diagnostics.hpp"

namespace RoseGold::DirectX12
{
	CommandQueue::CommandQueue(ComPtr<ID3D12Device> aDevice, D3D12_COMMAND_LIST_TYPE aQueueType)
		: myQueueType(aQueueType)
		, myNextFenceValue((std::uint64_t(aQueueType) << 56) + 1)
		, myLastCompletedFenceValue(std::uint64_t(aQueueType) << 56)
		, myFenceEventHandle(INVALID_HANDLE_VALUE)
	{
		ZoneScoped;

		D3D12_COMMAND_QUEUE_DESC queueDescriptor = { };
		queueDescriptor.Type = aQueueType;
		queueDescriptor.NodeMask = 0;
		AssertAction(
			aDevice->CreateCommandQueue(&queueDescriptor, IID_PPV_ARGS(myCommandQueue.ReleaseAndGetAddressOf())),
			"Create command queue."
		);

		const wchar_t* queueName = nullptr;

		switch (aQueueType)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			queueName = L"Compute queue";
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			queueName = L"Copy queue";
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			queueName = L"Graphics queue";
			break;
		}

		if (queueName != nullptr)
			myCommandQueue->SetName(queueName);

#ifdef TRACY_ENABLE
		myProfilingContext = TracyD3D12Context(aDevice.Get(), myCommandQueue.Get());

		switch (aQueueType)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			TracyD3D12ContextName(myProfilingContext, "Compute queue", 13);
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			TracyD3D12ContextName(myProfilingContext, "Copy queue", 10);
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			TracyD3D12ContextName(myProfilingContext, "Graphics queue", 14);
			break;
		}
#endif

		AssertAction(
			aDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(myFence.ReleaseAndGetAddressOf())),
			"Create command queue fence."
		);

		if (queueName != nullptr)
			myFence->SetName(queueName);
		myFence->Signal(myLastCompletedFenceValue);

		myFenceEventHandle = CreateEventEx(NULL, nullptr, false, EVENT_ALL_ACCESS);
		Debug::Assert(myFenceEventHandle != INVALID_HANDLE_VALUE, "Event handle is valid.");
	}

	std::uint64_t CommandQueue::InsertSignal()
	{
		std::lock_guard lock(myFenceMutex);
		myCommandQueue->Signal(myFence.Get(), myNextFenceValue);
		return myNextFenceValue++;
	}

	bool CommandQueue::IsFenceComplete(std::uint64_t aFenceValue)
	{
		if (aFenceValue > myLastCompletedFenceValue)
			PollCurrentFenceValue();

		return aFenceValue <= myLastCompletedFenceValue;
	}

	void CommandQueue::InsertWait(std::uint64_t aFenceValue)
	{
		myCommandQueue->Wait(myFence.Get(), aFenceValue);
	}

	void CommandQueue::InsertWaitForQueueFence(CommandQueue& anOtherQueue, std::uint64_t aFenceValue)
	{
		myCommandQueue->Wait(anOtherQueue.GetFence().Get(), aFenceValue);
	}

	void CommandQueue::InsertWaitForQueue(CommandQueue& anOtherQueue)
	{
		myCommandQueue->Wait(anOtherQueue.GetFence().Get(), anOtherQueue.GetNextFenceValue() - 1);
	}

	void CommandQueue::WaitForFenceCPUBlocking(std::uint64_t aFenceValue)
	{
		if (IsFenceComplete(aFenceValue))
			return;

		std::lock_guard lock(myEventMutex);
		myFence->SetEventOnCompletion(aFenceValue, myFenceEventHandle);
		WaitForSingleObjectEx(myFenceEventHandle, INFINITE, false);
		myLastCompletedFenceValue = aFenceValue;
	}

	std::uint64_t CommandQueue::PollCurrentFenceValue()
	{
		myLastCompletedFenceValue = std::max(myLastCompletedFenceValue, myFence->GetCompletedValue());
		return myLastCompletedFenceValue;
	}

	std::uint64_t CommandQueue::ExecuteCommandList(ComPtr<ID3D12CommandList> aCommandList)
	{
		ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
		if (!VerifyAction(aCommandList.As(&graphicsCommandList), "Convert to graphics command list.") ||
			!VerifyAction(graphicsCommandList->Close(), "Close command list."))
			return 0;

		myCommandQueue->ExecuteCommandLists(1, aCommandList.GetAddressOf());

		return InsertSignal();
	}

	CommandQueueManager::CommandQueueManager(ComPtr<ID3D12Device> aDevice)
		: myGraphicsQueue(aDevice, D3D12_COMMAND_LIST_TYPE_DIRECT)
		, myAsyncComputeQueue(aDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE)
		, myCopyQueue(aDevice, D3D12_COMMAND_LIST_TYPE_COPY)
	{
		ZoneScoped;

		myGraphicsQueue.GetCommandQueue()->SetName(L"Main graphics queue");
		myGraphicsQueue.GetFence()->SetName(L"Main graphics queue fence");
		myAsyncComputeQueue.GetCommandQueue()->SetName(L"Async compute queue");
		myAsyncComputeQueue.GetFence()->SetName(L"Async compute queue fence");
		myCopyQueue.GetCommandQueue()->SetName(L"Copy queue");
		myCopyQueue.GetFence()->SetName(L"Copy queue fence");
	}

	CommandQueue* CommandQueueManager::GetQueue(D3D12_COMMAND_LIST_TYPE aCommandType)
	{
		switch (aCommandType)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return &GetGraphicsQueue();
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return &GetComputeQueue();
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return &GetCopyQueue();
		default:
			Debug::LogFatal("Tried to get an unsupported command queue type.");
			return nullptr;
		}
	}

	bool CommandQueueManager::IsFenceComplete(std::uint64_t aFenceValue)
	{
		const D3D12_COMMAND_LIST_TYPE commandType = static_cast<D3D12_COMMAND_LIST_TYPE>(aFenceValue >> 56);
		return GetQueue(commandType)->IsFenceComplete(aFenceValue);
	}

	void CommandQueueManager::WaitForFenceCPUBlocking(std::uint64_t aFenceValue)
	{
		const D3D12_COMMAND_LIST_TYPE commandType = static_cast<D3D12_COMMAND_LIST_TYPE>(aFenceValue >> 56);
		return GetQueue(commandType)->WaitForFenceCPUBlocking(aFenceValue);
	}

	void CommandQueueManager::WaitForAllIdle()
	{
		myGraphicsQueue.WaitForIdle();
		myAsyncComputeQueue.WaitForIdle();
		myCopyQueue.WaitForIdle();
	}
}
