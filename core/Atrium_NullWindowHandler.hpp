#pragma once

#include "Atrium_WindowManagement.hpp"

namespace Atrium::Core
{
	class NullWindowManager final : public WindowManager
	{
	public:
		std::shared_ptr<Window> NewWindow(const CreationParameters&) { return nullptr; }

		std::vector<std::shared_ptr<Window>> GetWindows() const { return {}; }

		void Update() {}

		bool SupportsMultipleWindows() const { return false; }
	};
}
