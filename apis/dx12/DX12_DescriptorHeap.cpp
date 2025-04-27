#include "DX12_DescriptorHeap.hpp"
#include "DX12_Diagnostics.hpp"

namespace Atrium::DirectX12
{
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetCPUHandle() const
	{
		if (IsValid())
			return myData->myCPUHandle;
		else
			return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetCPUHandle(unsigned int anIndex) const
	{
		if (IsValid())
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = myData->myCPUHandle;
			handle.ptr += (myData->myHeap->GetDescriptorSize() * anIndex);
			return handle;
		}
		else
		{
			return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
		}
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetGPUHandle() const
	{
		if (IsValid())
			return myData->myGPUHandle;
		else
			return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapHandle::GetGPUHandle(unsigned int anIndex) const
	{
		if (IsValid())
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = myData->myGPUHandle;
			handle.ptr += (myData->myHeap->GetDescriptorSize() * anIndex);
			return handle;
		}
		else
		{
			return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		}
	}

	DescriptorHeapHandle::HandleData::~HandleData()
	{
		if (myHeap)
			myHeap->FreeHeapHandle(myHeapIndex);
	}

	void DescriptorHeapHandle::Invalidate()
	{
		myData.reset();
	}

	DescriptorHeap::DescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors, bool anIsReferencedByShader)
		: myHeapType(aHeapType)
		, myMaxDescriptors(aNumDescriptors)
		, myIsReferencedByShader(anIsReferencedByShader)
		, myDescriptorSize(0)
		, myDescriptorHeapCPUStart({ 0 })
		, myDescriptorHeapGPUStart({ 0 })
	{
		ZoneScoped;

		D3D12_DESCRIPTOR_HEAP_DESC heapDescriptor;
		heapDescriptor.NumDescriptors = myMaxDescriptors;
		heapDescriptor.Type = aHeapType;
		heapDescriptor.Flags = myIsReferencedByShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDescriptor.NodeMask = 0;

		if (!Debug::Verify(aDevice->CreateDescriptorHeap(
			&heapDescriptor,
			IID_PPV_ARGS(myDescriptorHeap.ReleaseAndGetAddressOf())),
			"Create descriptor heap."))
			return;

		myDescriptorHeapCPUStart = myDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (myIsReferencedByShader)
			myDescriptorHeapGPUStart = myDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		myDescriptorSize = aDevice->GetDescriptorHandleIncrementSize(myHeapType);
	}

	DescriptorHeapHandle DescriptorHeap::CreateHeapHandle(D3D12_CPU_DESCRIPTOR_HANDLE aCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE aGPUHandle, std::uint32_t aHeapIndex)
	{
		DescriptorHeapHandle handle;
		handle.myData.reset(new DescriptorHeapHandle::HandleData());
		handle.myData->myHeap = this;
		handle.myData->myCPUHandle = aCPUHandle;
		handle.myData->myGPUHandle = aGPUHandle;
		handle.myData->myHeapIndex = aHeapIndex;
		return handle;
	}

	StagingDescriptorHeap::StagingDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors)
		: DescriptorHeap(aDevice, aHeapType, aNumDescriptors, false)
		, myCurrentDescriptorIndex(0)
		, myActiveHandleCount(0)
	{
		myDescriptorHeap->SetName(L"Staging descriptor heap");
	}

	StagingDescriptorHeap::~StagingDescriptorHeap()
	{
		Debug::Assert(myActiveHandleCount == 0, "Should not contain active handles upon destruction.");
		myFreeDescriptors.clear();
	}

	DescriptorHeapHandle StagingDescriptorHeap::GetNewHeapHandle()
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

		myActiveHandleCount++;

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = myDescriptorHeapCPUStart;
		cpuHandle.ptr += newHandleId * myDescriptorSize;
		return CreateHeapHandle(cpuHandle, { 0 }, newHandleId);
	}

	void StagingDescriptorHeap::FreeHeapHandle(std::uint32_t anIndex)
	{
		Debug::Assert(myActiveHandleCount != 0, "No more handles left to free. Is something being freed incorrectly?");
		myFreeDescriptors.push_back(anIndex);

		myActiveHandleCount--;
	}

	RenderPassDescriptorHeap::RenderPassDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors)
		: DescriptorHeap(aDevice, aHeapType, aNumDescriptors, true)
		, myCurrentDescriptorIndex(0)
	{
		myDescriptorHeap->SetName(L"Render pass descriptor heap");
	}

	void RenderPassDescriptorHeap::Reset()
	{
		myCurrentDescriptorIndex = 0;
	}

	DescriptorHeapHandle RenderPassDescriptorHeap::GetHeapHandleBlock(std::uint32_t aCount)
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

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = myDescriptorHeapCPUStart;
		cpuHandle.ptr += newHandleID * myDescriptorSize;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = myDescriptorHeapGPUStart;
		gpuHandle.ptr += newHandleID * myDescriptorSize;

		return CreateHeapHandle(cpuHandle, gpuHandle, newHandleID);
	}
}
