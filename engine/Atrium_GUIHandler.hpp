// Filter "Editor"

#pragma once

#include <Core_InputDevice.hpp>
#include <Core_GUIContext.hpp>

// Dear ImGUI context.
struct ImGuiContext;

namespace Atrium
{
	namespace Core
	{
		class RenderTexture;
		class Window;
	}

	class GUIHandler
	{
	public:
		GUIHandler(const std::shared_ptr<Core::Window>& aWindow, const std::shared_ptr<Core::RenderTexture>& aRenderTarget, std::function<void()> anImGuiRenderCallback);
		~GUIHandler();

		Core::InputDeviceType GetAllowedInputs() const;

		void Render();

		#if IS_IMGUI_ENABLED
	private:
		void StyleColorsNord();

		std::unique_ptr<Core::GUIContext> myGUIContext;
		ImGuiContext* myImGuiContext;

		std::function<void()> myImGuiRenderCallback;
		bool myHasWindow;
		#endif
	};
}
