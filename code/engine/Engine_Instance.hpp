#pragma once

#include "Graphics_Manager.hpp"

#include "Platform_WindowManagement.hpp"

#include <memory>

namespace RoseGold
{
	class Game;
	class ImGuiHandler;

	class EngineInstance
	{
	public:
		static std::unique_ptr<EngineInstance> Create();

	public:
		~EngineInstance();

		Core::GraphicsAPI& GetGraphicsAPI() { return *myGraphicsAPI; }

		Core::WindowManager& GetWindowManager() { return *myWindowManager; }

		void InitializeImGui(Core::Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);

		void Run(Game& aGame);

		void Stop();

	private:
		EngineInstance();

	private:
		Game* myCurrentGame;
		bool myIsGameRunning;
		std::unique_ptr<ImGuiHandler> myImGuiHandler;

		std::unique_ptr<Core::GraphicsAPI> myGraphicsAPI;
		std::unique_ptr<Core::WindowManager> myWindowManager;
	};
}