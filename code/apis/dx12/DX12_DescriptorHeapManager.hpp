#pragma once

#include "DX12_DescriptorHeap.hpp"

namespace RoseGold::DirectX12
{
	class DescriptorHeapManager
	{
	public:
		DescriptorHeapManager(ComPtr<ID3D12Device> aDevice)
			: mySRVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
			, myCBVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
			, myUAVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
			, mySamplerHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 128)
			, myRTVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32)
			, myDSVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 32)
			, myFrameHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32)
		{ }

		StagingDescriptorHeap& GetShaderResourceViewHeap() { return mySRVHeap; }
		StagingDescriptorHeap& GetConstantBufferViewHeap() { return myCBVHeap; }
		StagingDescriptorHeap& GetUnorderedAccessViewHeap() { return myUAVHeap; }
		StagingDescriptorHeap& GetSamplerHeap() { return mySamplerHeap; }

		StagingDescriptorHeap& GetRTVHeap() { return myRTVHeap; }
		StagingDescriptorHeap& GetDSVHeap() { return myDSVHeap; }

		RenderPassDescriptorHeap& GetFrameHeap() { return myFrameHeap; }

	private:
		StagingDescriptorHeap mySRVHeap;
		StagingDescriptorHeap myCBVHeap;
		StagingDescriptorHeap myUAVHeap;
		StagingDescriptorHeap mySamplerHeap;
		StagingDescriptorHeap myRTVHeap;
		StagingDescriptorHeap myDSVHeap;

		RenderPassDescriptorHeap myFrameHeap;
	};
}