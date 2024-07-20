#pragma once

#include "Graphics_Manager.hpp"

#include <memory>

namespace RoseGold::DirectX12
{
	std::unique_ptr<Core::GraphicsAPI> CreateDX12Manager();
}