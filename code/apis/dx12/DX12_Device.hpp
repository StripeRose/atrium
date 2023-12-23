#pragma once

#include "DX12_CommandQueue.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeapManager.hpp"
#include "DX12_Pipeline.hpp"
#include "DX12_SwapChain.hpp"

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
		~Device();

		void MarkFrameStart();

		std::shared_ptr<SwapChain> CreateRenderTextureForWindow(Core::Platform::Window& aWindow);
		std::shared_ptr<SwapChain> GetSwapChain(Core::Platform::Window& aWindow);
		std::vector<std::shared_ptr<SwapChain>> GetSwapChains();

		const DeviceParameters& GetParameters() const { return myParameters; }

		ComPtr<ID3D12Device> GetDevice() { return myDevice; }
		ComPtr<IDXGIFactory4> GetFactory() { return myDXGIFactory; }
		DescriptorHeapManager& GetDescriptorHeapManager() { return *myDescriptorHeapManager; }
		CommandQueueManager& GetCommandQueueManager() { return *myCommandQueues.get(); }
		Pipeline& GetPipeline() { return *myPipeline; }

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
		std::unique_ptr<CommandQueueManager> myCommandQueues;
		std::unique_ptr<DescriptorHeapManager> myDescriptorHeapManager;
		std::unique_ptr<Pipeline> myPipeline;

		std::mutex mySwapChainMutex;
		std::map<Core::Platform::Window*, std::shared_ptr<SwapChain>> myDrawSurfaceSwapChain;

		DeviceParameters myParameters;
		D3D_FEATURE_LEVEL myFeatureLevel;
	};
}
