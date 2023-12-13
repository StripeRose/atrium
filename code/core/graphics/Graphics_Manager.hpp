#pragma once

#include "Graphics_CommandBuffer.hpp"
#include "Graphics_RenderTexture.hpp"
#include "Graphics_Tasks.hpp"

namespace RoseGold::Core::Platform { class Window; }

namespace RoseGold::Core::Graphics
{
	class Manager
	{
	public:
		virtual ~Manager() = default;
		
		virtual std::shared_ptr<RenderTexture> CreateRenderTextureForWindow(Platform::Window& aWindow) = 0;

		virtual void ExecuteCommandBuffer(const CommandBuffer& aCommandBuffer) = 0;
		virtual void ExecuteTask(const GraphicsTask& aGraphicsTask) = 0;

		virtual bool SupportsMultipleWindows() const = 0;

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}