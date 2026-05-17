// Filter "Backends"

#include "DearImGui_Null.hpp"

#include <backends/imgui_impl_null.h>

namespace Atrium::Extension
{
	DearImGuiBackendContext_NullGraphics::DearImGuiBackendContext_NullGraphics()
	{
		ImGui_ImplNullRender_Init();
	}

	DearImGuiBackendContext_NullGraphics::~DearImGuiBackendContext_NullGraphics()
	{
		ImGui_ImplNullRender_Shutdown();
	}

	void DearImGuiBackendContext_NullGraphics::MarkFrameStart()
	{
		ImGui_ImplNullRender_NewFrame();
	}

	void DearImGuiBackendContext_NullGraphics::Render(Atrium::FrameGraphicsContext&)
	{
		ImGui_ImplNullRender_RenderDrawData(ImGui::GetDrawData());
	}

	DearImGuiBackendContext_NullPlatform::DearImGuiBackendContext_NullPlatform()
	{
		ImGui_ImplNullPlatform_Init();
	}

	DearImGuiBackendContext_NullPlatform::~DearImGuiBackendContext_NullPlatform()
	{
		ImGui_ImplNullPlatform_Shutdown();
	}

	void DearImGuiBackendContext_NullPlatform::MarkFrameStart()
	{
		ImGui_ImplNullPlatform_NewFrame();
	}
}

