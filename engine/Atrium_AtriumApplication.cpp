#include "Atrium_AtriumApplication.hpp"

#include "Core_AudioAPI.hpp"
#include "Core_Diagnostics.hpp"
#include "Core_GraphicsAPI.hpp"
#include "Core_InputDeviceAPI.hpp"
#include "Core_WindowManagement.hpp"

namespace Atrium
{
	namespace
	{
		AtriumApplication* ourRunningApplication(nullptr);
	}

	AtriumApplication::AtriumApplication()
		: myIsRunning(false)
		, myHasShutdownBeenRequested(false)
		, myLastExitCode(EXIT_SUCCESS)
	{
		CreateAPIHandlers();
		AssertAPIHandlersExist();

		SetupEngine();
	}

	AtriumApplication::~AtriumApplication()
	{
		// Clean up is handled in the destructor instead of before Run() returns,
		// to be able to rely on the derived class to have released all its objects first.

		CleanupEngine();
	}

	void AtriumApplication::Exit()
	{
		myHasShutdownBeenRequested = true;
		myLastExitCode = EXIT_SUCCESS;
	}

	void AtriumApplication::Exit(int anExitCode)
	{
		myHasShutdownBeenRequested = true;
		myLastExitCode = anExitCode;
	}

	AtriumApplication* AtriumApplication::GetRunningInstance()
	{
		return ourRunningApplication;
	}

	int AtriumApplication::Run()
	{
		if (!HandleStartup())
			return EXIT_FAILURE;

		myIsRunning = true;
		myLastExitCode = EXIT_SUCCESS;

		RunMainLoop();

		HandleShutdown();

		return myLastExitCode;
	}

	void AtriumApplication::HandleCloseRequest(bool&)
	{
		// The default implementation will always honor the request to close the application and not change the request.
		// We don't want to risk the application being unable to exit.
	}

	void AtriumApplication::SetupEngine()
	{
		// Core systems have been set up and are usable.
		// Prepare all sub-systems for use.

		Debug::Assert(
			!ourRunningApplication,
			"A single process only has one AtriumApplication instance running at one time."
		);

		ourRunningApplication = this;

		myFrameGraphics = myGraphicsAPI->CreateFrameGraphicsContext();
	}

	void AtriumApplication::RunMainLoop()
	{
		while (myIsRunning)
		{
			if (myHasShutdownBeenRequested)
			{
				HandleCloseRequest(myHasShutdownBeenRequested);
				myIsRunning = !myHasShutdownBeenRequested;
				myHasShutdownBeenRequested = false;
				continue;
			}

			DoTick();
		}
	}

	void AtriumApplication::DoTick()
	{
		myWindowManager->Update();

		myInputDeviceAPI->ReportInputEvents(~Core::InputDeviceType::Unknown);

		myGraphicsAPI->MarkFrameStart();

		HandleFrameLogic(*myFrameGraphics);

		myGraphicsAPI->MarkFrameEnd();
	}

	void AtriumApplication::CleanupEngine()
	{
		// Game has exited and cleaned up its data, and is expected to not use the sub-systems any more.
		// Time to clean them up before the run finally ends.

		myFrameGraphics.reset();

		ourRunningApplication = nullptr;
	}
}
