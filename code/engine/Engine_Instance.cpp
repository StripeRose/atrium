#include "stdafx.hpp"
#include "Engine_Instance.hpp"

#include "Engine_Game.hpp"

#include "Common_Diagnostics.hpp"

namespace RoseGold
{
	void EngineInstance::Run(Game& aGame)
	{
		Debug::Assert(myCurrentGame == nullptr, L"This engine instance is already running a game.");
		Debug::Assert(aGame.myEngineInstance == nullptr, L"The game is already assigned to an engine instance.");

		aGame.myEngineInstance = this;
		myCurrentGame = &aGame;

		RunGame();

		aGame.myEngineInstance = nullptr;
		myCurrentGame = nullptr;
	}
	
	EngineInstance::EngineInstance()
		: myCurrentGame(nullptr)
	{ }
		
	void EngineInstance::RunGame()
	{
		myCurrentGame->OnStart();

		while (!myCurrentGame->myHasRequestedShutdown)
		{
			myWindowManager->Update();
			myGraphicsAPI->MarkFrameStart();
			myCurrentGame->OnLoop();
			myGraphicsAPI->MarkFrameEnd();
		}

		myCurrentGame->OnExit();
	}
}