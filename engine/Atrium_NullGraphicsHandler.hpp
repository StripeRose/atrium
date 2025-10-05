// Filter "NullManagers"

#pragma once

#include "Atrium_GraphicsAPI.hpp"

namespace Atrium
{
	class NullGraphicsHandler final : public GraphicsAPI
	{
	public:
		NullGraphicsHandler();

		std::shared_ptr<FrameGraphicsContext> CreateFrameGraphicsContext() override;
		std::uint_least64_t GetCurrentFrameIndex() const override;
		ResourceManager& GetResourceManager() override;
		void MarkFrameStart() override;
		void MarkFrameEnd() override;
		bool SupportsMultipleWindows() const;

	private:
		std::uint_least64_t myFrameCounter;
	};
}