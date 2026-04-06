// Filter "Backends"

#if IS_IMGUI_BACKEND_WIN32
#include "DearImGui_Win32.hpp"

#include <Atrium_Diagnostics.hpp>

#include <windef.h>
#include <backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Atrium::Extension
{
	DearImGuiBackendContext_Win32::DearImGuiBackendContext_Win32(const std::shared_ptr<Atrium::Window>& aWindow)
		: myWindow(std::static_pointer_cast<Atrium::Win32::Window>(aWindow))
	{
		PROFILE_SCOPE();

		ImGuiContext* imguiContext = ImGui::GetCurrentContext();

		Atrium::Win32::Window* win32Window = static_cast<Atrium::Win32::Window*>(aWindow.get());

		win32Window->OnWindowMessage.Connect(this, [imguiContext](Win32::Window::AdditionalWndProcData& data) {
			ImGuiContext* previousContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(imguiContext);
			LPARAM result = ImGui_ImplWin32_WndProcHandler(data.WindowHandle, data.Message, data.WParam, data.LParam);
			ImGuiIO& io = ImGui::GetIO();
			data.BlockKeyboard = io.WantCaptureKeyboard;
			data.BlockMouse = io.WantCaptureMouse;
			data.BlockAllMessages = (result == TRUE);
			ImGui::SetCurrentContext(previousContext);
			});

		ImGui_ImplWin32_Init(std::any_cast<HWND>(aWindow->GetNativeHandle()));
	}

	DearImGuiBackendContext_Win32::~DearImGuiBackendContext_Win32()
	{
		PROFILE_SCOPE();

		std::shared_ptr<Atrium::Win32::Window> window = myWindow.lock();
		if (window)
		{
			window->OnClosed.Disconnect(this);
			window->OnWindowMessage.Disconnect(this);
		}
		ImGui_ImplWin32_Shutdown();
	}

	void DearImGuiBackendContext_Win32::MarkFrameStart()
	{
		ImGui_ImplWin32_NewFrame();
	}
}

#endif
