// Filter "Backends"

#pragma once

#include "DearImGui_Backend.hpp"

#include <Atrium_FrameContext.hpp>
#include <Atrium_RenderTexture.hpp>

namespace Atrium::Extension
{
	class DearImGuiBackendContext_NullGraphics : public DearImGuiBackendContext
	{
	public:
		DearImGuiBackendContext_NullGraphics();
		~DearImGuiBackendContext_NullGraphics() override;

		void MarkFrameStart() override;
		void Render(Atrium::FrameGraphicsContext& aFrameContext) override;
	};

	class DearImGuiBackendContext_NullPlatform : public DearImGuiBackendContext
	{
	public:
		DearImGuiBackendContext_NullPlatform();
		~DearImGuiBackendContext_NullPlatform() override;

		void MarkFrameStart() override;
	};
}
