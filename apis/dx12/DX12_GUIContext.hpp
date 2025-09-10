// Filter "GUI"

#include "DX12_FrameContext.hpp"

#include <Atrium_FrameContext.hpp>
#include <Atrium_GUIContext.hpp>
#include <Atrium_RenderTexture.hpp>
#include <Atrium_WindowManagement.hpp>

#if IS_IMGUI_ENABLED
#include <DX12_DescriptorHeap.hpp>
#endif

namespace Atrium::DirectX12
{
	class DirectX12API;
	class GUIBackendContext : public Atrium::GUIContext
	{
	public:
		GUIBackendContext(DirectX12API& aGraphicsAPI, FrameGraphicsContext& aGraphicsContext, const std::shared_ptr<Atrium::RenderTexture>& aRenderTarget);
		~GUIBackendContext() override;

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		FrameGraphicsContext& myGraphicsContext;
		std::shared_ptr<Atrium::RenderTexture> myRenderTarget;
		std::unique_ptr<RenderPassDescriptorHeap> myCBV_SRVHeap;
		std::map<std::uint64_t, DescriptorHeapHandle> myCBV_SRVHeapHandles;
	};
}
