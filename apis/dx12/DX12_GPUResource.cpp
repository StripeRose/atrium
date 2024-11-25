// Filter "Resources"

#include "DX12_GPUResource.hpp"

#include "DX12_Diagnostics.hpp"

#include "D3D12MemAlloc.h"

namespace Atrium::DirectX12
{
	GPUResource::GPUResource(const ComPtr<ID3D12Resource>& aResource, D3D12_RESOURCE_STATES aUsageState)
		: myResource(aResource)
		, myUsageState(aUsageState)
	{
		Debug::Assert(aResource != nullptr, "Requires a valid resource.");
	}

	GPUResource::GPUResource(const ComPtr<D3D12MA::Allocation>& anAllocation, D3D12_RESOURCE_STATES aUsageState)
		: myAllocation(anAllocation)
		, myUsageState(aUsageState)
	{
		Debug::Assert(anAllocation != nullptr, "Requires a valid allocation.");

		myResource = anAllocation->GetResource();
	}

	GPUResource::~GPUResource()
	{

	}

	ComPtr<ID3D12Resource> GPUResource::GetResource() const
	{
		return myResource;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GPUResource::GetGPUAddress() const
	{
		return GetResource()->GetGPUVirtualAddress();
	}

	std::optional<D3D12_RESOURCE_BARRIER> GPUResource::UpdateUsageState(D3D12_RESOURCE_STATES aNewState)
	{
		if (myUsageState != aNewState)
		{
			D3D12_RESOURCE_BARRIER barrier;

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = GetResource().Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = myUsageState;
			barrier.Transition.StateAfter = aNewState;

			myUsageState = aNewState;

			return barrier;
		}
		else if (aNewState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER barrier;

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.UAV.pResource = GetResource().Get();

			return barrier;
		}

		return { };
	}

	void GPUResource::SetName(const std::wstring_view& aName)
	{
		if (myAllocation)
			myAllocation->SetName(aName.data());
		else
			myResource->SetName(aName.data());
	}

	void GPUResource::SetResource(const ComPtr<ID3D12Resource>& aResource, D3D12_RESOURCE_STATES aCurrentUsageState)
	{
		if (myAllocation)
			myAllocation->SetResource(aResource.Get());

		myResource = aResource;
		myUsageState = aCurrentUsageState;
	}
}
