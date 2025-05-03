#include "Atrium_Instance.hpp"

#include "Atrium_ImGui.hpp"

#include "Atrium_Diagnostics.hpp"

namespace Atrium
{
	EngineInstance::~EngineInstance()
	{
	}

	void EngineInstance::InitializeImGui(Window& aPrimaryWindow, std::shared_ptr<RenderTexture> aTarget)
	{
		Debug::Assert(!myImGuiHandler, "Only one primary ImGui window supported.");
		myImGuiHandler.reset(new ImGuiHandler(*myGraphicsAPI, aPrimaryWindow, aTarget));
		aPrimaryWindow.Closed.Connect(this, [&](Window&) { myImGuiHandler.reset(); });
	}

	void EngineInstance::Run()
	{
		myIsRunning = true;
		OnStart.Invoke();

		while (myIsRunning)
		{
			myWindowManager->Update();

			myGraphicsAPI->MarkFrameStart();

			InputDeviceType allowedInputDevices = ~InputDeviceType::Unknown;

			if (myImGuiHandler)
			{
				myImGuiHandler->MarkFrameStart();
				allowedInputDevices = myImGuiHandler->GetAllowedInputs();
			}

			myInputDeviceAPI->ReportInputEvents(allowedInputDevices);

			OnLoop.Invoke();

			if (myImGuiHandler)
			{
				OnImGui.Invoke();
				myImGuiHandler->MarkFrameEnd();
			}

			myGraphicsAPI->MarkFrameEnd();
		}

		myImGuiHandler.reset();
		OnExit.Invoke();
	}

	void EngineInstance::Stop()
	{
		myIsRunning = false;
	}
	
	EngineInstance::EngineInstance()
		: myIsRunning(false)
	{ }
}