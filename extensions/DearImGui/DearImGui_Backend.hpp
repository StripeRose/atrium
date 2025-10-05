// Filter "Backends"

#pragma once

#include <Atrium_FrameContext.hpp>
#include <Atrium_RenderTexture.hpp>
#include <Atrium_WindowManagement.hpp>

#include <memory>
#include <vector>

namespace Atrium::Extension
{
	class DearImGuiBackendContext
	{
	public:
		virtual ~DearImGuiBackendContext() = default;

		static void CreateBackendHandlers(
			const std::shared_ptr<Window>& aWindow,
			const std::shared_ptr<RenderTexture>& aRenderTarget,
			std::vector<std::unique_ptr<DearImGuiBackendContext>>& outBackends
		);

		virtual void MarkFrameStart() { };
		virtual void Render([[maybe_unused]] Atrium::FrameGraphicsContext& aFrameContext) { };
	};
}
