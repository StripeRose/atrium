// Filter "Graphics"

#pragma once

#include "Atrium_GraphicsAPI.hpp"

namespace Atrium::Core
{
	class NullGraphicsHandler final : public GraphicsAPI
	{
	public:
		NullGraphicsHandler();

		FrameGraphicsContext& GetCurrentFrameContext() override;
		std::uint_least64_t GetCurrentFrameIndex() const override;
		ResourceManager& GetResourceManager() override;
		void MarkFrameStart() override;
		void MarkFrameEnd() override;
		bool SupportsMultipleWindows() const;

	private:
		std::uint_least64_t myFrameCounter;
	};
}