// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"

#include <d3d12.h>

#include <optional>

namespace RoseGold::DirectX12
{
	class GPUResource
	{
		friend class UploadContext;
	public:
		virtual ~GPUResource() = default;

		ComPtr<ID3D12Resource> GetResource() const { return myResource; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return myResource->GetGPUVirtualAddress(); }
		D3D12_RESOURCE_STATES GetUsageState() const { return myUsageState; }

		[[nodiscard]]
		std::optional<D3D12_RESOURCE_BARRIER> UpdateUsageState(D3D12_RESOURCE_STATES aNewState)
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
				barrier.UAV.pResource = myResource.Get();

				return barrier;
			}

			return { };
		}

		bool IsReady() const { return myIsReady; }

	protected:
		ComPtr<ID3D12Resource> myResource;
		D3D12_RESOURCE_STATES myUsageState = D3D12_RESOURCE_STATE_COMMON;
		bool myIsReady = false;
	};
}
