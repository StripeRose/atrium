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

		myWindow->AdditionalWndProc = [](Win32::Window::AdditionalWndProcData& data) {
			LPARAM result = ImGui_ImplWin32_WndProcHandler(data.WindowHandle, data.Message, data.WParam, data.LParam);
			ImGuiIO& io = ImGui::GetIO();
			data.BlockKeyboard = io.WantCaptureKeyboard;
			data.BlockMouse = io.WantCaptureMouse;
			data.BlockAllMessages = (result == TRUE);
			};

		ImGui_ImplWin32_Init(std::any_cast<HWND>(aWindow->GetNativeHandle()));
	}

	DearImGuiBackendContext_Win32::~DearImGuiBackendContext_Win32()
	{
		PROFILE_SCOPE();
		myWindow->OnClosed.Disconnect(this);
		myWindow->AdditionalWndProc = nullptr;
		ImGui_ImplWin32_Shutdown();
	}

	void DearImGuiBackendContext_Win32::MarkFrameStart()
	{
		ImGui_ImplWin32_NewFrame();
	}
}

#endif
