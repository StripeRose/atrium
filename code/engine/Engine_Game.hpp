#pragma once

#include "Editor_GUI.hpp"
#include "Engine_Instance.hpp"

namespace RoseGold
{
	class EngineInstance;
	class Game
	{
		friend EngineInstance;

	public:
		~Game();

		void Run();

		void ShutDown();

	protected:
		virtual void OnStart() { }
		virtual void OnLoop() { }
		virtual void OnImGui() { }
		virtual void OnExit() { }

	protected:
		EngineInstance& GetEngine() { return *myEngineInstance; }
		const EngineInstance& GetEngine() const { return *myEngineInstance; }

		Core::GraphicsAPI& GetGraphicsAPI() { return myEngineInstance->GetGraphicsAPI(); }

		Core::WindowManager& GetWindowManager() { return myEngineInstance->GetWindowManager(); }

	private:
		std::unique_ptr<EngineInstance> myEngineInstance;
	};
}