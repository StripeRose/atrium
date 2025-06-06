#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_RenderTexture.hpp"

#include "D3D12MemAlloc.h"

#ifdef IS_DEBUG_BUILD
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#endif

namespace Atrium::DirectX12
{
	Device::Device(const DeviceParameters& someParameters)
		: myFeatureLevel(someParameters.MinimumFeatureLevel)
	{
		ZoneScoped;

		myParameters = someParameters;

		UINT dxgiFactoryFlags = 0;

#ifdef IS_DEBUG_BUILD
		SetupDebug(dxgiFactoryFlags);
#endif
		SetupFactory(dxgiFactoryFlags);
		SetupAdapter();
		SetupDevice();
		SetupAllocator();
		SetupInfoQueue();
		SetupHeapManager();
	}

	Device::~Device()
	{
		myDescriptorHeapManager.reset();
	}

	std::shared_ptr<GPUResource> Device::CreateResource(const D3D12_RESOURCE_DESC* aResourceDesc, D3D12_RESOURCE_STATES anInitialState, const D3D12_CLEAR_VALUE* aClearValue, D3D12_HEAP_TYPE aHeapType)
	{
		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType = aHeapType;

		ComPtr<D3D12MA::Allocation> allocation;
		HRESULT creationResult = myAllocator->CreateResource(
			&allocationDesc,
			aResourceDesc,
			anInitialState,
			aClearValue,
			allocation.ReleaseAndGetAddressOf(),
			IID_NULL, NULL
		);
		
		if (Debug::Verify(creationResult, "Create resource"))
			return std::shared_ptr<GPUResource>(new GPUResource(allocation, anInitialState));
		else
			return { };
	}

#if _DEBUG
	void Device::SetupDebug(UINT& someDXGIFlagsOut)
	{
		ComPtr<ID3D12Debug> debugController;
		if (Debug::Verify(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())), "Enable debug layer"))
			debugController->EnableDebugLayer();

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			someDXGIFlagsOut |= DXGI_CREATE_FACTORY_DEBUG;

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);

			//DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			//{
			//	80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
			//};
			//DXGI_INFO_QUEUE_FILTER filter = {};
			//filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			//filter.DenyList.pIDList = hide;
			//dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
	}
#endif

	bool Device::SetupFactory(UINT someDXGIFlags)
	{
		ZoneScoped;

		if (!Debug::Verify(CreateDXGIFactory2(someDXGIFlags, IID_PPV_ARGS(myDXGIFactory.GetAddressOf())), "Create DXGI Factory"))
			return false;

		if (myParameters.AllowTearing)
		{
			BOOL allowTearing = FALSE;

			ComPtr<IDXGIFactory5> dxgiFactory5;
			HRESULT factory5Result = myDXGIFactory.As(&dxgiFactory5);
			if (SUCCEEDED(factory5Result))
				factory5Result = dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

			if (FAILED(factory5Result) || allowTearing)
				myParameters.AllowTearing = false;
		}

		return true;
	}

	bool Device::SetupAdapter()
	{
		ZoneScoped;

		ComPtr<IDXGIFactory6> dxgiFactory6;
		HRESULT hr = myDXGIFactory.As(&dxgiFactory6);
		if (SUCCEEDED(hr))
		{
			for (UINT adapterIndex = 0;
				SUCCEEDED(dxgiFactory6->EnumAdapterByGpuPreference(
					adapterIndex,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(myAdapter.ReleaseAndGetAddressOf())));
				adapterIndex++)
			{
				DXGI_ADAPTER_DESC1 adapterDescription;
				if (FAILED(myAdapter->GetDesc1(&adapterDescription)))
					continue;

				if ((adapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
				{
					// Skip Basic Render Driver.
					continue;
				}

				// Find out if the adapter supports DX12.
				if (SUCCEEDED(D3D12CreateDevice(myAdapter.Get(), myParameters.MinimumFeatureLevel, __uuidof(ID3D12Device), nullptr)))
				{
					Debug::Log("Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n",
						adapterIndex,
						adapterDescription.VendorId,
						adapterDescription.DeviceId,
						adapterDescription.Description);
					break;
				}
			}
		}

		if (!myAdapter)
		{
			for (UINT adapterIndex = 0;
				SUCCEEDED(myDXGIFactory->EnumAdapters1(
					adapterIndex,
					myAdapter.ReleaseAndGetAddressOf()));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 adapterDescription;
				if (FAILED(myAdapter->GetDesc1(&adapterDescription)))
					continue;

				if ((adapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
				{
					// Skip Basic Render Driver.
					continue;
				}

				// Find out if the adapter supports DX12.
				if (SUCCEEDED(D3D12CreateDevice(myAdapter.Get(), myParameters.MinimumFeatureLevel, __uuidof(ID3D12Device), nullptr)))
				{
					Debug::Log("Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n",
						adapterIndex,
						adapterDescription.VendorId,
						adapterDescription.DeviceId,
						adapterDescription.Description);
					break;
				}
			}
		}

		if (!myAdapter)
		{
			// Try WARP12 instead
			if (SUCCEEDED(myDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(myAdapter.ReleaseAndGetAddressOf()))))
				Debug::Log("Direct3D Adapter - WARP12\n");
		}

		if (!myAdapter)
		{
			Debug::LogError("No DirectX 12 capable device found.");
			return false;
		}

		return true;
	}

	bool Device::SetupDevice()
	{
		ZoneScoped;

		if (!Debug::Verify(D3D12CreateDevice(
			myAdapter.Get(),
			myParameters.MinimumFeatureLevel,
			IID_PPV_ARGS(myDevice.ReleaseAndGetAddressOf())), "Create DirectX 12 device."))
			return false;

		myDevice->SetName(L"DX12_Device");

		return true;
	}

	bool Device::SetupAllocator()
	{
		ZoneScoped;

		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = myDevice.Get();
		allocatorDesc.pAdapter = myAdapter.Get();
		allocatorDesc.Flags
			= D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED
			| D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;

		return Debug::Verify(
			D3D12MA::CreateAllocator(&allocatorDesc, myAllocator.ReleaseAndGetAddressOf()),
			"Create memory allocator"
		);
	}

	bool Device::SetupInfoQueue()
	{
		ZoneScoped;

#ifndef NDEBUG
		if (!Debug::Verify(myDevice.As(&myInfoQueue), "Get info queue."))
			return false;

		//D3D12_MESSAGE_ID hide[] =
		//{
		//	D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		//	D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		//	// Workarounds for debug layer issues on hybrid-graphics systems
		//	D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
		//	D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
		//};
		//D3D12_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
		//filter.DenyList.pIDList = hide;
		//myInfoQueue->AddStorageFilterEntries(&filter);

#endif

		return true;
	}

	bool Device::FindMaximumFeatureLevel()
	{
		ZoneScoped;

		static const D3D_FEATURE_LEVEL s_featureLevels[] =
		{
#if defined(NTDDI_WIN10_FE) || defined(USING_D3D12_AGILITY_SDK)
			D3D_FEATURE_LEVEL_12_2,
#endif
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
		{
			static_cast<UINT>(std::size(s_featureLevels)), s_featureLevels, D3D_FEATURE_LEVEL_11_0
		};

		if (SUCCEEDED(myDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels))))
			myFeatureLevel = featLevels.MaxSupportedFeatureLevel;
		else
			myFeatureLevel = myParameters.MinimumFeatureLevel;

		return true;
	}

	bool Device::SetupHeapManager()
	{
		ZoneScoped;

		myDescriptorHeapManager.reset(new DescriptorHeapManager(myDevice, DX12_FRAMES_IN_FLIGHT));
		return myDescriptorHeapManager.get() != nullptr;
	}
}
