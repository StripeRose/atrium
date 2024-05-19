#pragma once

#include "DX12_DescriptorHeap.hpp"

#include <array>

namespace RoseGold::DirectX12
{
	class DescriptorHeapManager
	{
	public:
		DescriptorHeapManager(ComPtr<ID3D12Device> aDevice, std::size_t aNumberOfFramesInFlight);

		void MarkFrameStart(std::uint64_t aFrameIndex);

		StagingDescriptorHeap& GetShaderResourceViewHeap() { return mySRVHeap; }
		StagingDescriptorHeap& GetConstantBufferViewHeap() { return myCBVHeap; }
		StagingDescriptorHeap& GetUnorderedAccessViewHeap() { return myUAVHeap; }
		StagingDescriptorHeap& GetSamplerHeap() { return mySamplerHeap; }

		StagingDescriptorHeap& GetRTVHeap() { return myRTVHeap; }
		StagingDescriptorHeap& GetDSVHeap() { return myDSVHeap; }

		RenderPassDescriptorHeap& GetFrameHeap() { return *myFrameHeaps[myFrameInFlight]; }

	private:
		StagingDescriptorHeap mySRVHeap;
		StagingDescriptorHeap myCBVHeap;
		StagingDescriptorHeap myUAVHeap;
		StagingDescriptorHeap mySamplerHeap;
		StagingDescriptorHeap myRTVHeap;
		StagingDescriptorHeap myDSVHeap;

		std::vector<std::unique_ptr<RenderPassDescriptorHeap>> myFrameHeaps;

		std::uint_least8_t myFrameInFlight;
	};
}