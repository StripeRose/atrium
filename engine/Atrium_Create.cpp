#include "Atrium_AtriumApplication.hpp"

#include "Atrium_Diagnostics.hpp"

#include "Atrium_NullAudioHandler.hpp"
#include "Atrium_NullGraphicsHandler.hpp"
#include "Atrium_NullInputHandler.hpp"
#include "Atrium_NullWindowHandler.hpp"

#ifdef ATRIUM_DX12
#include "DX12_Instancer.hpp"
#endif

#ifdef ATRIUM_VULKAN
#include "VK_Instancer.hpp"
#endif

#ifdef ATRIUM_WIN32
#include "Win32_WindowManagement.hpp"
#include "Win32_InputDeviceAPI.hpp"
#endif

namespace Atrium
{
	// This function sets up all core API handlers that the rest of the engine is building off of, depending on the current platform.
	void AtriumApplication::CreateAPIHandlers()
	{
		PROFILE_SCOPE();

	#if ATRIUM_WIN32

		switch (myApplicationParameters.Graphics)
		{
			case ApplicationParameters::None:
				break;

			#ifdef ATRIUM_DX12
			case ApplicationParameters::DirectX12:
				myGraphicsAPI.reset(DirectX12::CreateDX12Manager().release());
				break;
			#endif

			#ifdef ATRIUM_VULKAN
			case ApplicationParameters::Vulkan:
				myGraphicsAPI.reset(Vulkan::CreateVulkanManager(
					myApplicationParameters.Name,
					myApplicationParameters.Version
				).release());
				break;
			#endif
		}

		myInputDeviceAPI.reset(new Win32::InputDeviceAPI());
		myWindowManager.reset(new Win32::WindowManager());

	#elif !defined(IGNORE_NOOP_PLATFORM)

		Debug::LogFatal(
			"This platform deos not have any core implementations. Atrium will not do anything.\n"
			"If this is intentional, define \"IGNORE_NOOP_PLATFORM\" to disable this error."
		);

	#endif

		// Populate null objects for missing API handlers.
		if (!myAudioAPI) myAudioAPI.reset(new NullAudioHandler());
		if (!myInputDeviceAPI) myInputDeviceAPI.reset(new NullInputHandler());
		if (!myGraphicsAPI)
		{
			myGraphicsAPI.reset(new NullGraphicsHandler());
			myApplicationParameters.Graphics = ApplicationParameters::None;
		}
		if (!myWindowManager) myWindowManager.reset(new NullWindowManager());
	}

	void AtriumApplication::AssertAPIHandlersExist()
	{
		// To avoid needing null-checks later, these basic systems must have an instance of some kind of handler.
		// If the functionality isn't desired or isn't supported on a particular platform, they should have null objects populated.
		Debug::Assert(!!myAudioAPI, "Application is expected to have an audio API handler.");
		Debug::Assert(!!myGraphicsAPI, "Application is expected to have a graphics API handler.");
		Debug::Assert(!!myInputDeviceAPI, "Application is expected to have an input device API handler.");
		Debug::Assert(!!myWindowManager, "Application is expected to have a window API handler.");
	}
}