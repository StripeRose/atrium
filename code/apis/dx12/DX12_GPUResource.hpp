// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"

#include <d3d12.h>

namespace RoseGold::DirectX12
{
	class GPUResource
	{
	public:
		virtual ~GPUResource() = default;

		ComPtr<ID3D12Resource> GetResource() const { return myResource; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return myResource->GetGPUVirtualAddress(); }
		D3D12_RESOURCE_STATES GetUsageState() const { return myUsageState; }

		bool IsReady() const { return myIsReady; }

	protected:
		ComPtr<ID3D12Resource> myResource;
		D3D12_RESOURCE_STATES myUsageState = D3D12_RESOURCE_STATE_COMMON;
		bool myIsReady = false;
	};
}
