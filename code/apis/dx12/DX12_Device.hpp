#pragma once

#include "DX12_ComPtr.hpp"

#include <Platform_WindowManagement.hpp>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <map>
#include <mutex>

namespace RoseGold::DirectX12
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

		void CreateSwapChain(Core::Platform::Window& aWindow);

	private:
#ifdef _DEBUG
		void SetupDebug(UINT& someDXGIFlagsOut);
#endif
		bool SetupFactory(UINT someDXGIFlags);
		bool SetupAdapter();
		bool SetupDevice();
		bool SetupInfoQueue();
		bool FindMaximumFeatureLevel();
		bool SetupCommandQueue();
		bool SetupHeapManager();

	private:
		ComPtr<IDXGIFactory4> myDXGIFactory;
		ComPtr<IDXGIAdapter1> myAdapter;
		ComPtr<ID3D12Device> myDevice;
		ComPtr<ID3D12InfoQueue> myInfoQueue;
		//std::unique_ptr<CommandQueueManager> myCommandQueues;
		//std::unique_ptr<DescriptorHeapManager> myDescriptorHeapManager;

		/*std::mutex mySwapChainMutex;
		std::map<Platform::DrawSurface*, std::unique_ptr<SwapChain>> myDrawSurfaceSwapChain;
		SwapChain* myPrimarySwapChain;*/

		DeviceParameters myParameters;
		D3D_FEATURE_LEVEL myFeatureLevel;
	};
}
