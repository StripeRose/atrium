// Filter "NullManagers"

#pragma once

#include "Atrium_WindowManagement.hpp"

namespace Atrium
{
	class NullWindowManager final : public WindowManager
	{
	public:
		std::unique_ptr<GUIContext> CreateGUIContext(const std::shared_ptr<Window>&) { return nullptr; }

		std::shared_ptr<Window> NewWindow() { return nullptr; }

		void Update() {}

		bool SupportsMultipleWindows() const { return false; }
	};
}
