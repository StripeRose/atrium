#pragma once

#include "Atrium_GraphicsAPI.hpp"
#include "Atrium_InputDeviceAPI.hpp"
#include "Atrium_WindowManagement.hpp"

#include <memory>

namespace Atrium
{
	class ImGuiHandler;

	class EngineInstance
	{
	public:
		static std::unique_ptr<EngineInstance> Create();

	public:
		~EngineInstance();

		Core::GraphicsAPI& GetGraphicsAPI() { return *myGraphicsAPI; }

		Core::InputDeviceAPI& GetInputAPI() { return *myInputDeviceAPI; }

		WindowManager& GetWindowManager() { return *myWindowManager; }

		void InitializeImGui(Window& aPrimaryWindow, std::shared_ptr<Core::RenderTexture> aTarget);

		void Run();

		void Stop();

	public:
		RoseCommon::EventSlot<> OnStart;
		RoseCommon::EventSlot<> OnLoop;
		RoseCommon::EventSlot<> OnImGui;
		RoseCommon::EventSlot<> OnExit;

	private:
		EngineInstance();

	private:
		bool myIsRunning;
		std::unique_ptr<ImGuiHandler> myImGuiHandler;

		std::unique_ptr<Core::GraphicsAPI> myGraphicsAPI;
		std::unique_ptr<Core::InputDeviceAPI> myInputDeviceAPI;
		std::unique_ptr<WindowManager> myWindowManager;
	};
}