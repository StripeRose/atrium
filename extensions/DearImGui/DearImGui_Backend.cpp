// Filter "Backends"

#include "DearImGui_Backend.hpp"

#include "Atrium_AtriumApplication.hpp"
#include "DearImGui_Null.hpp"

#if ATRIUM_DX12
#include "DearImGui_DirectX12.hpp"
#endif

#if ATRIUM_WIN32
#include "DearImGui_Win32.hpp"
#endif

namespace Atrium::Extension
{
	void DearImGuiBackendContext::CreateBackendHandlers(const std::shared_ptr<Window>& aWindow, const std::shared_ptr<RenderTexture>& aRenderTarget, std::vector<std::unique_ptr<DearImGuiBackendContext>>& outBackends)
	{
	#if ATRIUM_WIN32
		switch (AtriumApplication::GetRunningInstance()->GetParameters().Graphics)
		{
			#if ATRIUM_DX12
			case ApplicationParameters::DirectX12:
				outBackends.emplace_back(new DearImGuiBackendContext_DirectX12(aRenderTarget));
				break;
			#endif

			/*case ApplicationParameters::Vulkan:
			{
				outBackends.emplace_back(new DearImGuiBackendContext_Vulkan(aRenderTarget));
				break;
			}*/

			default:
				outBackends.emplace_back(new DearImGuiBackendContext_NullGraphics());
				break;
		}

		outBackends.emplace_back(new DearImGuiBackendContext_Win32(aWindow));
	#else
		outBackends.emplace_back(new DearImGuiBackendContext_NullPlatform());
	#endif
	}
}
