// Filter "ImGui"

#include "DX12_FrameContext.hpp"

#include <Core_FrameContext.hpp>
#include <Core_ImGuiContext.hpp>
#include <Core_RenderTexture.hpp>
#include <Core_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <DX12_DescriptorHeap.hpp>
#endif

namespace Atrium::DirectX12
{
	class DirectX12API;
	class ImGuiContext : public Core::ImGuiContext
	{
	public:
		ImGuiContext(DirectX12API& aGraphicsAPI, FrameGraphicsContext& aGraphicsContext, const std::shared_ptr<Core::RenderTexture>& aRenderTarget);
		~ImGuiContext() override;

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		FrameGraphicsContext& myGraphicsContext;
		std::shared_ptr<Core::RenderTexture> myRenderTarget;
		std::unique_ptr<RenderPassDescriptorHeap> myCBV_SRVHeap;
	};
}
