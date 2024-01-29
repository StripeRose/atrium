#include "stdafx.hpp"
#include "Bootstrapper.hpp"

#include <DX12_Manager.hpp>
#include <Win32_WindowManagement.hpp>

namespace RoseGold::Client
{
	void Bootstrap_Windows(BootstrapResult& aResult)
	{
		aResult.GraphicsManager.reset(new DirectX12::Manager());
		aResult.WindowManager.reset(new Win32::WindowManager());
	}

	BootstrapResult Bootstrap()
	{
		BootstrapResult result;

#if _WIN32
		Bootstrap_Windows(result);
#else
		static_assert(false, "No bootstrapping available for this platform yet.");
#endif

		return result;
	}
}
