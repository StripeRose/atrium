#include "stdafx.hpp"
#include "Engine_Instance.hpp"

#include "Engine_Game.hpp"
#include "Engine_ImGui.hpp"

#include "Common_Diagnostics.hpp"

namespace RoseGold
{
	EngineInstance::~EngineInstance()
	{
	}

	void EngineInstance::InitializeImGui(Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget)
	{
		Debug::Assert(!myImGuiHandler, "Only one primary ImGui window supported.");
		myImGuiHandler.reset(new ImGuiHandler(*myGraphicsAPI, aPrimaryWindow, aTarget));
	}

	void EngineInstance::Run(Game& aGame)
	{
		Debug::Assert(myCurrentGame == nullptr, L"This engine instance is already running a game.");

		myCurrentGame = &aGame;
		myCurrentGame->OnStart();
		myIsGameRunning = true;

		while (myIsGameRunning)
		{
			myWindowManager->Update();

			myGraphicsAPI->MarkFrameStart();

			if (myImGuiHandler)
				myImGuiHandler->MarkFrameStart();

			myCurrentGame->OnLoop();

			if (myImGuiHandler)
				myImGuiHandler->MarkFrameEnd();

			myGraphicsAPI->MarkFrameEnd();
		}

		myImGuiHandler.reset();
		myCurrentGame->OnExit();
		myCurrentGame = nullptr;
	}

	void EngineInstance::Stop()
	{
		myIsGameRunning = false;
	}
	
	EngineInstance::EngineInstance()
		: myCurrentGame(nullptr)
		, myIsGameRunning(false)
	{ }
}