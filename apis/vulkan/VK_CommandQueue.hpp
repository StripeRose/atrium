#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Atrium::Vulkan
{
	class Device;
	class CommandQueueManager
	{
	public:
		CommandQueueManager(vk::raii::Queue aGraphicsQueue);

	private:
		vk::raii::Queue myGraphicsQueue = nullptr;
	};
}