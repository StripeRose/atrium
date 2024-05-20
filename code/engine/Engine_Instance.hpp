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
		static std::unique_ptr<EngineInstance> Create();

	public:
		~EngineInstance();

		Core::GraphicsAPI& GetGraphicsAPI() { return *myGraphicsAPI; }

		Core::WindowManager& GetWindowManager() { return *myWindowManager; }

		void Run(Game& aGame);

		void Stop();

	private:
		EngineInstance();

	private:
		Game* myCurrentGame;
		bool myIsGameRunning;

		std::unique_ptr<Core::GraphicsAPI> myGraphicsAPI;
		std::unique_ptr<Core::WindowManager> myWindowManager;
	};
}