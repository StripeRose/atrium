#include "stdafx.hpp"
#include "Engine_Game.hpp"

#include "Common_Diagnostics.hpp"

namespace RoseGold
{
	Game::~Game()
	{
		Debug::Assert(myEngineInstance == nullptr, L"Game cannot still be assigned to an engine instance. It might still be running.");
	}

	void Game::RequestShutDown()
	{
		myHasRequestedShutdown = true;
	}
}