#include "Atrium_AtriumApplication.hpp"

#include "Core_Diagnostics.hpp"

#include "Core_NullAudioHandler.hpp"
#include "Core_NullGraphicsHandler.hpp"
#include "Core_NullInputHandler.hpp"
#include "Core_NullWindowHandler.hpp"

#if _WIN32
#include "DX12_Instancer.hpp"
#include "Win32_WindowManagement.hpp"
#include "Win32_InputDeviceAPI.hpp"
#endif

namespace Atrium
{
	// This function sets up all core API handlers that the rest of the engine is building off of, depending on the current platform.
	void AtriumApplication::CreateAPIHandlers()
	{
		ZoneScoped;

		#if _WIN32

		myGraphicsAPI.reset(DirectX12::CreateDX12Manager().release());
		myInputDeviceAPI.reset(new Win32::InputDeviceAPI());
		myWindowManager.reset(new Win32::WindowManager());

		#elif !defined(IGNORE_NOOP_PLATFORM)

		Debug::LogFatal(
			"This platform deos not have any core implementations. Atrium will not do anything.\n"
			"If this is intentional, define \"IGNORE_NOOP_PLATFORM\" to disable this error."
		);

		#endif

		// Populate null objects for missing API handlers.
		if (!myAudioAPI) myAudioAPI.reset(new Core::NullAudioHandler());
		if (!myInputDeviceAPI) myInputDeviceAPI.reset(new Core::NullInputHandler());
		if (!myGraphicsAPI) myGraphicsAPI.reset(new Core::NullGraphicsHandler());
		if (!myWindowManager) myWindowManager.reset(new Core::NullWindowManager());
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