#pragma once

#include "Graphics_Manager.hpp"

#include "Platform_WindowManagement.hpp"

#include <memory>

namespace RoseGold
{
	class Game;

	class EngineInstance
	{
	public:
		static EngineInstance Create();

	public:
		Core::GraphicsAPI& GetGraphicsAPI() { return *myGraphicsAPI; }

		Core::WindowManager& GetWindowManager() { return *myWindowManager; }

		void Run(Game& aGame);

	private:
		EngineInstance();

		void RunGame();

	private:
		Game* myCurrentGame;

		std::unique_ptr<Core::GraphicsAPI> myGraphicsAPI;
		std::unique_ptr<Core::WindowManager> myWindowManager;
	};
}