#pragma once

#include "Graphics_Manager.hpp"

#include "Platform_WindowManagement.hpp"

#include <memory>

namespace RoseGold::Client
{
	struct BootstrapResult
	{
		// Todo: Add eventual graphics, audio and input handlers.
		std::unique_ptr<Core::GraphicsAPI> GraphicsAPI;
		std::unique_ptr<Core::WindowManager> WindowManager;
	};

	BootstrapResult Bootstrap();
}
