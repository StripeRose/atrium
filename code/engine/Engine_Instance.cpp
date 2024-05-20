#include "stdafx.hpp"
#include "Engine_Instance.hpp"

#include "Engine_Game.hpp"

#include "Common_Diagnostics.hpp"

namespace RoseGold
{
	EngineInstance::~EngineInstance()
	{
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
			myCurrentGame->OnLoop();
			myGraphicsAPI->MarkFrameEnd();
		}

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