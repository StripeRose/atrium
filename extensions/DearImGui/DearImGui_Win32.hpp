// Filter "Backends"

#pragma once

#if IS_IMGUI_BACKEND_WIN32
#include "DearImGui_Backend.hpp"

#include <Win32_WindowManagement.hpp>

namespace Atrium::Extension
{
	class DearImGuiBackendContext_Win32 : public DearImGuiBackendContext
	{
	public:
		DearImGuiBackendContext_Win32(const std::shared_ptr<Atrium::Window>& aWindow);
		~DearImGuiBackendContext_Win32() override;

		void MarkFrameStart() override;

	private:
		std::shared_ptr<Atrium::Win32::Window> myWindow;
	};
}

#endif
