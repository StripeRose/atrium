#include "DX12_DescriptorHeap.hpp"

#include "DX12_Diagnostics.hpp"

namespace RoseGold::DirectX12
{
	DescriptorHeapHandle::~DescriptorHeapHandle()
	{
		if (myHeap)
			myHeap->FreeHeapHandle(*this);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetCPUHandle(unsigned int anIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = myCPUHandle;
		handle.ptr += (myHeap->GetDescriptorSize() * anIndex);
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetGPUHandle(unsigned int anIndex) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = myGPUHandle;
		handle.ptr += (myHeap->GetDescriptorSize() * anIndex);
		return handle;
	}

	DescriptorHeap::DescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors, bool anIsReferencedByShader)
		: myHeapType(aHeapType)
		, myMaxDescriptors(aNumDescriptors)
		, myIsReferencedByShader(anIsReferencedByShader)
		, myDescriptorSize(0)
		, myDescriptorHeapCPUStart({ 0 })
		, myDescriptorHeapGPUStart({ 0 })
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDescriptor;
		heapDescriptor.NumDescriptors = myMaxDescriptors;
		heapDescriptor.Type = aHeapType;
		heapDescriptor.Flags = myIsReferencedByShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDescriptor.NodeMask = 0;

		if (!VerifyAction(aDevice->CreateDescriptorHeap(
			&heapDescriptor,
			IID_PPV_ARGS(myDescriptorHeap.ReleaseAndGetAddressOf())),
			"Create descriptor heap."))
			return;

		myDescriptorHeapCPUStart = myDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (myIsReferencedByShader)
			myDescriptorHeapGPUStart = myDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		myDescriptorSize = aDevice->GetDescriptorHandleIncrementSize(myHeapType);
	}

	std::shared_ptr<DescriptorHeapHandle> DescriptorHeap::CreateHeapHandle()
	{
		return std::shared_ptr<DescriptorHeapHandle>(new DescriptorHeapHandle(this));
	}

	StagingDescriptorHeap::StagingDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors)
		: DescriptorHeap(aDevice, aHeapType, aNumDescriptors, false)
		, myCurrentDescriptorIndex(0)
		, myActiveHandleCount(0)
	{ }

	StagingDescriptorHeap::~StagingDescriptorHeap()
	{
		Debug::Assert(myActiveHandleCount == 0, "No active handles remain.");
		myFreeDescriptors.clear();
	}

	std::shared_ptr<DescriptorHeapHandle> StagingDescriptorHeap::GetNewHeapHandle()
	{
		std::uint32_t newHandleId = 0;

		if (myCurrentDescriptorIndex < myMaxDescriptors)
		{
			newHandleId = myCurrentDescriptorIndex;
			myCurrentDescriptorIndex++;
		}
		else if (!myFreeDescriptors.empty())
		{
			newHandleId = myFreeDescriptors.at(myFreeDescriptors.size() - 1);
			myFreeDescriptors.erase(myFreeDescriptors.begin() + (myFreeDescriptors.size() - 1));
		}
		else
		{
			Debug::LogFatal("Ran out of dynamic descriptor heap handles.");
		}

		std::shared_ptr<DescriptorHeapHandle> newHandle = CreateHeapHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = myDescriptorHeapCPUStart;
		cpuHandle.ptr += newHandleId * myDescriptorSize;
		newHandle->SetCPUHandle(cpuHandle);
		newHandle->SetHeapIndex(newHandleId);
		myActiveHandleCount++;

		return newHandle;
	}

	void StagingDescriptorHeap::FreeHeapHandle(const DescriptorHeapHandle& handle)
	{
		Debug::Assert(myActiveHandleCount != 0, "Still have handles left to free.");
		myFreeDescriptors.push_back(handle.GetHeapIndex());

		myActiveHandleCount--;
	}

	RenderPassDescriptorHeap::RenderPassDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors)
		: DescriptorHeap(aDevice, aHeapType, aNumDescriptors, true)
		, myCurrentDescriptorIndex(0)
	{ }

	void RenderPassDescriptorHeap::Reset()
	{
		myCurrentDescriptorIndex = 0;
	}

	std::shared_ptr<DescriptorHeapHandle> RenderPassDescriptorHeap::GetHeapHandleBlock(std::uint32_t aCount)
	{
		std::uint32_t newHandleID = 0;
		std::uint32_t blockEnd = myCurrentDescriptorIndex + aCount;

		if (blockEnd < myMaxDescriptors)
		{
			newHandleID = myCurrentDescriptorIndex;
			myCurrentDescriptorIndex = blockEnd;
		}
		else
		{
			Debug::LogFatal("Ran out of render pass descriptor heap handles, need to increase heap size.");
		}

		std::shared_ptr<DescriptorHeapHandle> newHandle = CreateHeapHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = myDescriptorHeapCPUStart;
		cpuHandle.ptr += newHandleID * myDescriptorSize;
		newHandle->SetCPUHandle(cpuHandle);

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = myDescriptorHeapGPUStart;
		gpuHandle.ptr += newHandleID * myDescriptorSize;
		newHandle->SetGPUHandle(gpuHandle);

		newHandle->SetHeapIndex(newHandleID);

		return newHandle;
	}
}
