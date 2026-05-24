#pragma once

#include "Atrium_GraphicsAPI.hpp"

#include <memory>
#include <string_view>

namespace Atrium::Vulkan
{
	std::unique_ptr<Atrium::GraphicsAPI> CreateVulkanManager(std::string_view anApplicationName, SemanticVersion anApplicationVersion);
}