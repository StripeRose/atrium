// Filter "Editor"

#pragma once

#include <Atrium_InputDevice.hpp>
#include <Atrium_GUIContext.hpp>

// Dear ImGUI context.
struct ImGuiContext;

namespace Atrium
{
	class RenderTexture;
	class Window;

	class GUIHandler
	{
	public:
		GUIHandler(const std::shared_ptr<Window>& aWindow, const std::shared_ptr<RenderTexture>& aRenderTarget, std::function<void()> anImGuiRenderCallback);
		~GUIHandler();

		InputDeviceType GetAllowedInputs() const;

		void Render();

		#if IS_IMGUI_ENABLED
	private:
		void StyleColorsNord();

		std::unique_ptr<GUIContext> myGUIContext;
		ImGuiContext* myImGuiContext;

		std::function<void()> myImGuiRenderCallback;
		bool myHasWindow;
		#endif
	};
}
