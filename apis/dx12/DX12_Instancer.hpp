#pragma once

#include "Core_GraphicsAPI.hpp"

#include <memory>

namespace Atrium::DirectX12
{
	std::unique_ptr<Atrium::Core::GraphicsAPI> CreateDX12Manager();
}