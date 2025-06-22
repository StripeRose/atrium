// Filter "Graphics"

#pragma once

#include "Core_GraphicsAPI.hpp"

namespace Atrium::Core
{
	class NullGraphicsHandler final : public GraphicsAPI
	{
	public:
		NullGraphicsHandler();

		std::shared_ptr<Core::FrameGraphicsContext> CreateFrameGraphicsContext() override;
		std::unique_ptr<ImGuiContext> CreateImGuiContext(const std::shared_ptr<Core::RenderTexture>&) override;
		std::uint_least64_t GetCurrentFrameIndex() const override;
		ResourceManager& GetResourceManager() override;
		void MarkFrameStart() override;
		void MarkFrameEnd() override;
		bool SupportsMultipleWindows() const;

	private:
		std::uint_least64_t myFrameCounter;
	};
}