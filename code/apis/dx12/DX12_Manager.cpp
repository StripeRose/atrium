#include "DX12_Manager.hpp"

#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"

#include "Common_Debug.hpp"

#ifdef _DEBUG
#include <dxgi1_6.h>
#include <dxgidebug.h>
#endif

namespace RoseGold::DirectX12
{
	Manager::Manager()
	{
		Debug::Log("DX12 start");
		myDevice.reset(new Device());
	}

	Manager::~Manager()
	{
		Debug::Log("DX12 stop");

		myDevice.reset();
		ReportUnreleasedObjects();
	}

	void Manager::CreateSwapChain(Core::Platform::Window& aWindow)
	{
		myDevice->CreateSwapChain(aWindow);
	}

	void Manager::ReportUnreleasedObjects()
	{
#if _DEBUG
		ComPtr<IDXGIDebug1> debugInterface;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debugInterface.GetAddressOf()))))
		{
			//debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
#endif
	}
}
