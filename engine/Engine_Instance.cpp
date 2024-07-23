#include "Engine_Instance.hpp"

#include "Engine_ImGui.hpp"

#include "Core_Diagnostics.hpp"

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