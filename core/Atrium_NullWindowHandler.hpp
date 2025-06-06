#pragma once

#include "Core_WindowManagement.hpp"

namespace Atrium::Core
{
	class NullWindowManager final : public WindowManager
	{
	public:
		std::shared_ptr<Window> NewWindow() { return nullptr; }

		void Update() {}

		bool SupportsMultipleWindows() const { return false; }
	};
}
