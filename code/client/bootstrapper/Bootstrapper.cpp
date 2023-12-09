#include "Bootstrapper.hpp"

#include <Win32_WindowManagement.hpp>

namespace RoseGold::Client
{
	void Bootstrap_Windows(BootstrapResult& aResult)
	{
		aResult.WindowManager.reset(new Platform::Win32::WindowManager());
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
