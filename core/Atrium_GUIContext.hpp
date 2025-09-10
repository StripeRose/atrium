#pragma once

#include <memory>
#include <vector>

namespace Atrium
{
	class GUIContext
	{
	public:
		virtual ~GUIContext() = default;

		static std::unique_ptr<GUIContext> Composite(std::vector<std::unique_ptr<GUIContext>>&& someContexts);

		virtual void MarkFrameStart() = 0;
		virtual void MarkFrameEnd() = 0;
	};
}