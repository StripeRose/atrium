// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Diagnostics.hpp"

#include <d3d12.h>

#include <optional>

namespace Atrium::DirectX12
{
	class GPUResource final
	{
		friend class UploadContext;
	public:
		GPUResource(ComPtr<ID3D12Resource> aResource, D3D12_RESOURCE_STATES anInitialUsageState)
			: myResource(aResource)
			, myUsageState(anInitialUsageState)
		{
			Debug::Assert(aResource, "GPU Resource is always created with a valid DX12 resource.");
		}

		virtual ~GPUResource() = default;

		ComPtr<ID3D12Resource> GetResource() const { return myResource; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return myResource->GetGPUVirtualAddress(); }
		D3D12_RESOURCE_STATES GetUsageState() const { return myUsageState; }

		void SetName(const std::wstring_view& aName) { myResource->SetName(aName.data()); }

		void SetResource(ComPtr<ID3D12Resource> aResource, D3D12_RESOURCE_STATES aCurrentUsageState)
		{
			myResource = aResource;
			myUsageState = aCurrentUsageState;
		}

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

	protected:
		ComPtr<ID3D12Resource> myResource;
		D3D12_RESOURCE_STATES myUsageState = D3D12_RESOURCE_STATE_COMMON;
	};
}
