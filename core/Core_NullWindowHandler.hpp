#pragma once

#include "Core_WindowManagement.hpp"

namespace Atrium::Core
{
	class NullWindowManager final : public WindowManager
	{
	public:
		std::unique_ptr<ImGuiContext> CreateImGuiContext(const std::shared_ptr<Core::Window>&) { return nullptr; }

		std::shared_ptr<Window> NewWindow() { return nullptr; }

		void Update() {}

		bool SupportsMultipleWindows() const { return false; }
	};
}
