// Filter "Backends"

#include "DearImGui_Backend.hpp"

#if IS_IMGUI_BACKEND_DIRECTX12
#include "DearImGui_DirectX12.hpp"
#endif

#if IS_IMGUI_BACKEND_WIN32
#include "DearImGui_Win32.hpp"
#endif

namespace Atrium::Extension
{
	void DearImGuiBackendContext::CreateBackendHandlers(const std::shared_ptr<Window>& aWindow, const std::shared_ptr<RenderTexture>& aRenderTarget, std::vector<std::unique_ptr<DearImGuiBackendContext>>& outBackends)
	{
		#if IS_IMGUI_BACKEND_DIRECTX12
		outBackends.emplace_back(new DearImGuiBackendContext_DirectX12(aRenderTarget));
		#endif

		#if IS_IMGUI_BACKEND_WIN32
		outBackends.emplace_back(new DearImGuiBackendContext_Win32(aWindow));
		#endif
	}
}
