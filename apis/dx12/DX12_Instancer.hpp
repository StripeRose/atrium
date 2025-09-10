#pragma once

#include "Atrium_GraphicsAPI.hpp"

#include <memory>

namespace Atrium::DirectX12
{
	std::unique_ptr<Atrium::GraphicsAPI> CreateDX12Manager();
}