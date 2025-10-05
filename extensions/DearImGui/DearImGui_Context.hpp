#pragma once

#include <Atrium_FrameContext.hpp>
#include <Atrium_InputDevice.hpp>
#include <Atrium_RenderTexture.hpp>
#include <Atrium_WindowManagement.hpp>

struct ImGuiContext;

namespace Atrium::Extension
{
	class DearImGuiBackendContext;

	class DearImGuiContext
	{
	public:
		DearImGuiContext(const std::shared_ptr<Window>& aWindow, const std::shared_ptr<RenderTexture>& aRenderTarget, std::function<void()> anImGuiRenderCallback);
		~DearImGuiContext();

		InputDeviceType GetAllowedInputs() const;

		void Render(Atrium::FrameGraphicsContext& aFrameContext);

	private:
		void StyleColorsNord();

		ImGuiContext* myImGuiContext;

		std::vector<std::unique_ptr<DearImGuiBackendContext>> myBackends;
		std::function<void()> myImGuiRenderCallback;
		bool myHasWindow;
	};
}