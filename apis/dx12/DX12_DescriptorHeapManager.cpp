#include "DX12_DescriptorHeapManager.hpp"

#include <string>

namespace RoseGold::DirectX12
{
	DescriptorHeapManager::DescriptorHeapManager(ComPtr<ID3D12Device> aDevice, std::size_t aNumberOfFramesInFlight)
		: mySRVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
		, myCBVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
		, myUAVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128)
		, mySamplerHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 128)
		, myRTVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32)
		, myDSVHeap(aDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 32)
		, myFrameInFlight(static_cast<std::uint_least8_t>(-1))
	{
		myFrameHeaps.resize(aNumberOfFramesInFlight);

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

	void DescriptorHeapManager::MarkFrameStart(std::uint64_t aFrameIndex)
	{
		myFrameInFlight = static_cast<std::uint_least8_t>(aFrameIndex % myFrameHeaps.size());

		GetFrameHeap().Reset();
	}
}