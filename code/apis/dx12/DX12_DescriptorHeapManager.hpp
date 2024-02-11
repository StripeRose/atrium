#pragma once

#include "DX12_DescriptorHeap.hpp"

#include <array>

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
			, myFrameInFlight(static_cast<std::uint_least8_t>(-1))
		{
			for (std::size_t i = 0; i < myFrameHeaps.size(); ++i)
			{
				myFrameHeaps[i] = std::make_unique<RenderPassDescriptorHeap>(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32);
				myFrameHeaps[i]->GetHeap()->SetName((std::wstring(L"Frame heap #") + std::to_wstring(i)).c_str());
			}

			mySRVHeap.GetHeap()->SetName(L"Staging descriptor heap SRV");
			myCBVHeap.GetHeap()->SetName(L"Staging descriptor heap CBV");
			myUAVHeap.GetHeap()->SetName(L"Staging descriptor heap UAV");
			mySamplerHeap.GetHeap()->SetName(L"Staging descriptor heap sampler");
			myRTVHeap.GetHeap()->SetName(L"Staging descriptor heap RTV");
			myDSVHeap.GetHeap()->SetName(L"Staging descriptor heap DSV");
		}

		void MarkFrameStart(std::uint64_t aFrameIndex)
		{
			myFrameInFlight = aFrameIndex % DX12_FRAMES_IN_FLIGHT;

			GetFrameHeap().Reset();
		}

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

		std::array<std::unique_ptr<RenderPassDescriptorHeap>, DX12_FRAMES_IN_FLIGHT> myFrameHeaps;

		std::uint_least8_t myFrameInFlight;
	};
}