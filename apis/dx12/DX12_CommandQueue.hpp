// Filter "Commands"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Diagnostics.hpp"

#include <d3d12.h>

#include <mutex>
#include <span>

namespace Atrium::DirectX12
{
	class Device;
	class CommandQueue
	{
	public:
		CommandQueue(ComPtr<ID3D12Device> aDevice, D3D12_COMMAND_LIST_TYPE aQueueType = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandQueue();

		std::uint64_t InsertSignal();

		bool IsFenceComplete(std::uint64_t aFenceValue);
		void InsertWait(std::uint64_t aFenceValue);
		void InsertWaitForQueueFence(CommandQueue& anOtherQueue, std::uint64_t aFenceValue);
		void InsertWaitForQueue(CommandQueue& anOtherQueue);

		void WaitForFenceCPUBlocking(std::uint64_t aFenceValue);
		void WaitForIdle() { WaitForFenceCPUBlocking(myNextFenceValue - 1); }

		ComPtr<ID3D12CommandQueue> GetCommandQueue() { return myCommandQueue; }
		D3D12_COMMAND_LIST_TYPE GetQueueType() const { return myQueueType; }

#ifdef TRACY_ENABLE
		TracyD3D12Ctx& GetProfilingContext() { return myProfilingContext; }
#endif

		std::uint64_t PollCurrentFenceValue();
		std::uint64_t GetLastCompletedFence() const { return myLastCompletedFenceValue; }
		std::uint64_t GetNextFenceValue() const { return myNextFenceValue; }
		ComPtr<ID3D12Fence> GetFence() { return myFence; }

		std::uint64_t ExecuteCommandList(ComPtr<ID3D12CommandList> aCommandList);
		std::uint64_t ExecuteCommandLists(std::span<ComPtr<ID3D12CommandList>> someCommandLists);

	private:
		ComPtr<ID3D12CommandQueue> myCommandQueue;
		D3D12_COMMAND_LIST_TYPE myQueueType;

#ifdef TRACY_ENABLE
		TracyD3D12Ctx myProfilingContext;
#endif

		std::mutex myFenceMutex;
		std::mutex myEventMutex;

		ComPtr<ID3D12Fence> myFence;
		std::uint64_t myNextFenceValue;
		std::uint64_t myLastCompletedFenceValue;
		HANDLE myFenceEventHandle;
	};

	class CommandQueueManager
	{
	public:
		CommandQueueManager(ComPtr<ID3D12Device> aDevice);

		CommandQueue& GetGraphicsQueue() { return myGraphicsQueue; }
		CommandQueue& GetComputeQueue() { return myAsyncComputeQueue; }
		CommandQueue& GetCopyQueue() { return myCopyQueue; }

		CommandQueue* GetQueue(D3D12_COMMAND_LIST_TYPE aCommandType);

		bool IsFenceComplete(std::uint64_t aFenceValue);
		void WaitForFenceCPUBlocking(std::uint64_t aFenceValue);
		void WaitForAllIdle();

	private:
		CommandQueue myGraphicsQueue;
		CommandQueue myAsyncComputeQueue;
		CommandQueue myCopyQueue;
	};
}