#include "stdafx.hpp"
#include "Engine_Game.hpp"

#include "Common_Diagnostics.hpp"

namespace RoseGold
{
	Game::~Game()
	{
	}

	void Game::Run()
	{
		myEngineInstance = EngineInstance::Create();
		myEngineInstance->Run(*this);
	}

	void Game::ShutDown()
	{
		myEngineInstance->Stop();
	}
}