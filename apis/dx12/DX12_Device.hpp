#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeapManager.hpp"
#include "DX12_GPUResource.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <map>
#include <mutex>

namespace D3D12MA { class Allocator; }

namespace Atrium::DirectX12
{
	class Device
	{
	public:
		struct DeviceParameters
		{
			bool AllowTearing = false;
			bool EnableHDR = false;
			D3D_FEATURE_LEVEL MinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		};

	public:
		Device(const DeviceParameters& someParameters = { });
		~Device();

		void MarkFrameStart(std::uint64_t aFrameIndex);

		std::shared_ptr<GPUResource> CreateResource(
			const D3D12_RESOURCE_DESC* aResourceDesc,
			D3D12_RESOURCE_STATES anInitialState,
			const D3D12_CLEAR_VALUE* aClearValue,
			D3D12_HEAP_TYPE aHeapType = D3D12_HEAP_TYPE_DEFAULT
			);

		const DeviceParameters& GetParameters() const { return myParameters; }

		ComPtr<ID3D12Device> GetDevice() { return myDevice; }
		ComPtr<IDXGIFactory4> GetFactory() { return myDXGIFactory; }
		DescriptorHeapManager& GetDescriptorHeapManager() { return *myDescriptorHeapManager; }

	private:
#ifdef IS_DEBUG_BUILD
		void SetupDebug(UINT& someDXGIFlagsOut);
#endif
		bool SetupFactory(UINT someDXGIFlags);
		bool SetupAdapter();
		bool SetupDevice();
		bool SetupAllocator();
		bool SetupInfoQueue();
		bool FindMaximumFeatureLevel();
		bool SetupHeapManager();

	private:
		ComPtr<IDXGIFactory4> myDXGIFactory;
		ComPtr<IDXGIAdapter1> myAdapter;
		ComPtr<ID3D12Device> myDevice;
		ComPtr<D3D12MA::Allocator> myAllocator;
		ComPtr<ID3D12InfoQueue> myInfoQueue;
		std::unique_ptr<DescriptorHeapManager> myDescriptorHeapManager;

		DeviceParameters myParameters;
		D3D_FEATURE_LEVEL myFeatureLevel;
	};
}
