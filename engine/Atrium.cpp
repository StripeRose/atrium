#include "Atrium.hpp"

#include "Atrium_AudioAPI.hpp"
#include "Atrium_Diagnostics.hpp"
#include "Atrium_GraphicsAPI.hpp"
#include "Atrium_InputDeviceAPI.hpp"
#include "Atrium_WindowManagement.hpp"

namespace Atrium
{
	AtriumApplication::AtriumApplication()
		: myIsRunning(false)
		, myHasShutdownBeenRequested(false)
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
	}

	bool AtriumApplication::Run()
	{
		if (!HandleStartup())
			return false;

		myIsRunning = true;

		RunMainLoop();

		HandleShutdown();

		return true;
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

		// Todo: Assert there is no other application instance created on the thread previously.

		// Todo: Prepare sub-system.
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

		// Todo: Clean up the sub-systems.
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
