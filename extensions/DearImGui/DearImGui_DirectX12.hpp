// Filter "Backends"

#pragma once

#if IS_IMGUI_BACKEND_DIRECTX12

#include "DearImGui_Backend.hpp"

#include <Atrium_FrameContext.hpp>
#include <Atrium_RenderTexture.hpp>

#include <DX12_DescriptorHeap.hpp>
#include <DX12_Manager.hpp>

namespace Atrium::Extension
{
	class DearImGuiBackendContext_DirectX12 : public DearImGuiBackendContext
	{
	public:
		DearImGuiBackendContext_DirectX12(const std::shared_ptr<Atrium::RenderTexture>& aRenderTarget);
		~DearImGuiBackendContext_DirectX12() override;

		void MarkFrameStart() override;
		void Render(Atrium::FrameGraphicsContext& aFrameContext) override;

	private:
		std::shared_ptr<Atrium::RenderTexture> myRenderTarget;
		std::unique_ptr<DirectX12::RenderPassDescriptorHeap> myCBV_SRVHeap;
		std::map<std::uint64_t, DirectX12::DescriptorHeapHandle> myCBV_SRVHeapHandles;
	};
}

#endif
