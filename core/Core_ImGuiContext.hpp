#pragma once

#include <memory>
#include <vector>

namespace Atrium::Core
{
	class ImGuiContext
	{
	public:
		virtual ~ImGuiContext() = default;

		static std::unique_ptr<ImGuiContext> Composite(std::vector<std::unique_ptr<ImGuiContext>>&& someContexts);

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}