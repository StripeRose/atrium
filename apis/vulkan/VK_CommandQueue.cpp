#include "VK_CommandQueue.hpp"
#include "VK_Device.hpp"

namespace Atrium::Vulkan
{
	CommandQueueManager::CommandQueueManager(vk::raii::Queue aGraphicsQueue)
		: myGraphicsQueue(aGraphicsQueue)
	{
		
	}
}
