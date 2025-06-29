// Filter "ImGui"

#include "Win32_ImGuiContext.hpp"

#include <Core_Diagnostics.hpp>

#if IS_IMGUI_ENABLED
#include <windef.h>
#include <backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace Atrium::Win32
{
	ImGuiContext::ImGuiContext(const std::shared_ptr<Core::Window>& aWindow)
		: myWindow(std::static_pointer_cast<Window>(aWindow))
	{
		ZoneScoped;

		#if IS_IMGUI_ENABLED
		myWindow->AdditionalWndProc = [](Win32::Window::AdditionalWndProcData& data) {
			LPARAM result = ImGui_ImplWin32_WndProcHandler(data.WindowHandle, data.Message, data.WParam, data.LParam);
			ImGuiIO& io = ImGui::GetIO();
			data.BlockKeyboard = io.WantCaptureKeyboard;
			data.BlockMouse = io.WantCaptureMouse;
			data.BlockAllMessages = (result == TRUE);
			};

		ImGui_ImplWin32_Init(std::any_cast<HWND>(aWindow->GetNativeHandle()));
		#endif
	}

	ImGuiContext::~ImGuiContext()
	{
		ZoneScoped;
		#if IS_IMGUI_ENABLED
		myWindow->OnClosed.Disconnect(this);
		myWindow->AdditionalWndProc = nullptr;
		ImGui_ImplWin32_Shutdown();
		#endif
	}

	void ImGuiContext::MarkFrameStart()
	{
		#if IS_IMGUI_ENABLED
		ImGui_ImplWin32_NewFrame();
		#endif
	}

	void ImGuiContext::MarkFrameEnd()
	{

	}
}
