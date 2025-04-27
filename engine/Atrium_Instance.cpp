#include "Atrium_Instance.hpp"

#include "Atrium_ImGui.hpp"

#include "Atrium_Diagnostics.hpp"

namespace Atrium
{
	EngineInstance::~EngineInstance()
	{
	}

	void EngineInstance::InitializeImGui(Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget)
	{
		Debug::Assert(!myImGuiHandler, "Only one primary ImGui window supported.");
		myImGuiHandler.reset(new ImGuiHandler(*myGraphicsAPI, aPrimaryWindow, aTarget));
		aPrimaryWindow.Closed.Connect(this, [&](Core::Window&) { myImGuiHandler.reset(); });
	}

	void EngineInstance::Run()
	{
		myIsRunning = true;
		OnStart.Invoke();

		while (myIsRunning)
		{
			myWindowManager->Update();

			myGraphicsAPI->MarkFrameStart();

			if (myImGuiHandler)
				myImGuiHandler->MarkFrameStart();

			myInputDeviceAPI->ReportInputEvents(myImGuiHandler->GetAllowedInputs());

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