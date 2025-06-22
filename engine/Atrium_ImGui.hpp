// Filter "Editor"

#pragma once

#include <Core_InputDevice.hpp>
#include <Core_ImGuiContext.hpp>

struct ImGuiContext;

namespace Atrium
{
	class Window;
	class ImGuiHandler
	{
	public:
		ImGuiHandler(const Window& aWindow, std::function<void()> anImGuiRenderCallback);
		~ImGuiHandler();

		Core::InputDeviceType GetAllowedInputs() const;

		void Render();

		#if IS_IMGUI_ENABLED
	private:
		void StyleColorsNord();

		std::unique_ptr<Core::ImGuiContext> myImGuiContexts;
		ImGuiContext* myImGuiContext;

		std::function<void()> myImGuiRenderCallback;
		#endif
	};
}
