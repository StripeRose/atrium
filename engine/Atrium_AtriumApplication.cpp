#include "Atrium_AtriumApplication.hpp"

#include "Core_AudioAPI.hpp"
#include "Core_Diagnostics.hpp"
#include "Core_GraphicsAPI.hpp"
#include "Core_InputDeviceAPI.hpp"
#include "Core_WindowManagement.hpp"

namespace Atrium
{
	Core::AudioAPI* ourAudioHandler(nullptr);
	Core::GraphicsAPI* ourGraphicsHandler(nullptr);
	Core::InputDeviceAPI* ourInputHandler(nullptr);
	Core::WindowManager* ourWindowHandler(nullptr);

	Core::FrameGraphicsContext* ourFrameGraphicsContext(nullptr);

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
			!ourAudioHandler && !ourGraphicsHandler && !ourInputHandler && !ourWindowHandler,
			"Handler instances expected to be empty upon startup. Are other Atrium applications running on the same thread?"
		);

		ourAudioHandler = myAudioAPI.get();
		ourGraphicsHandler = myGraphicsAPI.get();
		ourInputHandler = myInputDeviceAPI.get();
		ourWindowHandler = myWindowManager.get();

		myFrameGraphics = myGraphicsAPI->CreateFrameGraphicsContext();
		ourFrameGraphicsContext = myFrameGraphics.get();
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

		HandleFrameLogic();

		myGraphicsAPI->MarkFrameEnd();
	}

	void AtriumApplication::CleanupEngine()
	{
		// Game has exited and cleaned up its data, and is expected to not use the sub-systems any more.
		// Time to clean them up before the run finally ends.

		ourFrameGraphicsContext = nullptr;
		myFrameGraphics.reset();

		ourAudioHandler = nullptr;
		ourGraphicsHandler = nullptr;
		ourInputHandler = nullptr;
		ourWindowHandler = nullptr;
	}
}

//void EngineInstance::InitializeImGui(Window& aPrimaryWindow, std::shared_ptr<RenderTexture> aTarget)
//{
//	Debug::Assert(!myImGuiHandler, "Only one primary ImGui window supported.");
//	myImGuiHandler.reset(new ImGuiHandler(*myGraphicsAPI, aPrimaryWindow, aTarget));
//	aPrimaryWindow.Closed.Connect(this, [&](Window&) { myImGuiHandler.reset(); });
//}
//
//void EngineInstance::Run()
//{
//	myIsRunning = true;
//	OnStart.Invoke();
//
//	while (myIsRunning)
//	{
//		myWindowManager->Update();
//
//		myGraphicsAPI->MarkFrameStart();
//
//		InputDeviceType allowedInputDevices = ~InputDeviceType::Unknown;
//
//		if (myImGuiHandler)
//		{
//			myImGuiHandler->MarkFrameStart();
//			allowedInputDevices = myImGuiHandler->GetAllowedInputs();
//		}
//
//		myInputDeviceAPI->ReportInputEvents(allowedInputDevices);
//
//		OnLoop.Invoke();
//
//		if (myImGuiHandler)
//		{
//			OnImGui.Invoke();
//			myImGuiHandler->MarkFrameEnd();
//		}
//
//		myGraphicsAPI->MarkFrameEnd();
//	}
//
//	myImGuiHandler.reset();
//
//	OnExit.Invoke();
//}
