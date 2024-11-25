// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"

#include <d3d12.h>

#include <optional>
#include <string_view>

namespace D3D12MA { class Allocation; }

namespace Atrium::DirectX12
{
	class GPUResource final
	{
	public:
		GPUResource(const ComPtr<ID3D12Resource>& aResource, D3D12_RESOURCE_STATES aUsageState = D3D12_RESOURCE_STATE_COMMON);
		GPUResource(const ComPtr<D3D12MA::Allocation>& anAllocation, D3D12_RESOURCE_STATES aUsageState = D3D12_RESOURCE_STATE_COMMON);

		~GPUResource();

		ComPtr<ID3D12Resource> GetResource() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const;

		inline D3D12_RESOURCE_STATES GetUsageState() const { return myUsageState; }

		[[nodiscard]]
		std::optional<D3D12_RESOURCE_BARRIER> UpdateUsageState(D3D12_RESOURCE_STATES aNewState);

		void SetName(const std::wstring_view& aName);

		void SetResource(const ComPtr<ID3D12Resource>& aResource, D3D12_RESOURCE_STATES aCurrentUsageState);

	private:
		ComPtr<D3D12MA::Allocation> myAllocation;
		ComPtr<ID3D12Resource> myResource;
		D3D12_RESOURCE_STATES myUsageState;
	};
}
