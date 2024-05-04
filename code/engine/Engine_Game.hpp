#pragma once

#include "Engine_Instance.hpp"

namespace RoseGold
{
	class EngineInstance;
	class Game
	{
		friend EngineInstance;

	public:
		~Game();

	protected:
		virtual void OnStart() { }
		virtual void OnLoop() { }
		virtual void OnExit() { }

	protected:
		Core::GraphicsAPI& GetGraphicsAPI() { return myEngineInstance->GetGraphicsAPI(); }

		Core::WindowManager& GetWindowManager() { return myEngineInstance->GetWindowManager(); }

		void RequestShutDown();

	private:
		EngineInstance* myEngineInstance = nullptr;
		bool myHasRequestedShutdown = false;
	};
}