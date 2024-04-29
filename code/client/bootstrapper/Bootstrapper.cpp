#include "stdafx.hpp"

#include "Bootstrapper.hpp"

#include "Common_Diagnostics.hpp"

#include "DX12_Instancer.hpp"

#include "Win32_WindowManagement.hpp"

namespace RoseGold::Client
{
	void Bootstrap_Windows(BootstrapResult& aResult)
	{
		aResult.GraphicsAPI.reset(DirectX12::CreateDX12Manager().release());
		aResult.WindowManager.reset(new Win32::WindowManager());
	}

	BootstrapResult Bootstrap()
	{
		ZoneScoped;

		BootstrapResult result;

#if _WIN32
		Bootstrap_Windows(result);
#else
		static_assert(false, "No bootstrapping available for this platform yet.");
#endif

		return result;
	}
}
