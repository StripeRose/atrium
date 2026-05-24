#include "VK_Manager.hpp"
#include "VK_Device.hpp"

#include <algorithm>
#include <array>
#include <vector>

// Vulkan Documentation:
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/01_Presentation/00_Window_surface.html

namespace Atrium::Vulkan
{
	namespace
	{
		static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity,
			VkDebugUtilsMessageTypeFlagsEXT aType,
			const VkDebugUtilsMessengerCallbackDataEXT* someCallbackData,
			void* /*someUserData*/
		)
		{
			const char* messageType = "Unknown";

			switch (static_cast<unsigned int>(aType))
			{
				case 1: messageType = "General"; break;
				case 2: messageType = "Validation"; break;
				case 3: messageType = "General | Validation"; break;
				case 4: messageType = "Performance"; break;
				case 5: messageType = "General | Performance"; break;
				case 6: messageType = "Validation | Performance"; break;
				case 7: messageType = "General | Validation | Performance"; break;
			}

			const std::string message = std::format("[Vulkan] [{}]: {}", messageType, someCallbackData->pMessage);

			switch (aSeverity)
			{
				default:
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					Debug::Log(message.data());
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					Debug::LogWarning(message.data());
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					Debug::LogError(message.data());
					break;
			}

			return VK_FALSE;
		}
	}

	std::unique_ptr<GraphicsAPI> CreateVulkanManager(std::string_view anApplicationName, SemanticVersion anApplicationVersion)
	{
		return VulkanManager::Create(anApplicationName, anApplicationVersion);
	}

	std::unique_ptr<VulkanManager> VulkanManager::Create(std::string_view anApplicationName, SemanticVersion anApplicationVersion)
	{
		std::unique_ptr<VulkanManager> manager(new VulkanManager());

		vkb::InstanceBuilder instanceBuilder;
		auto builtInstance = instanceBuilder
			.set_engine_name("Atrium").set_engine_version(0, 0, 0)
			.set_app_name(anApplicationName.data())
			.set_app_version(anApplicationVersion.Major, anApplicationVersion.Minor, anApplicationVersion.Patch)

			.request_validation_layers()
			.set_debug_callback(
				[](VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type,
					const VkDebugUtilsMessengerCallbackDataEXT* data, void* userdata)
				{ return DebugCallback(sev, type, data, userdata); })

			.build();

		if (!builtInstance)
		{
			Debug::LogError("Could not create Vulkan instance: %s", builtInstance.error().message().c_str());
			return nullptr;
		}

		manager->myInstance = builtInstance.value();
		manager->myInstanceDispatch = manager->myInstance.make_table();

		vkb::PhysicalDeviceSelector gpuSelector(manager->myInstance);
		auto selectedGPU = gpuSelector.defer_surface_initialization().select();
		if (!selectedGPU)
		{
			Debug::LogError(selectedGPU.error().message().c_str());

			if (selectedGPU.error() == vkb::PhysicalDeviceError::no_suitable_device)
			{
				const auto& detailed_reasons = selectedGPU.detailed_failure_reasons();
				if (!detailed_reasons.empty())
				{
					Debug::LogError("GPU Selection failure reasons:");
					for (const std::string& reason : detailed_reasons)
						Debug::LogError(reason.c_str());
				}
			}
			return nullptr;
		}

		manager->myPhysicalDevice = selectedGPU.value();

		vkb::DeviceBuilder deviceBuilder(manager->myPhysicalDevice);
		auto builtDevice = deviceBuilder.build();

		if (!builtDevice)
		{
			Debug::LogError(builtDevice.error().message().c_str());
			return nullptr;
		}

		manager->myDevice = builtDevice.value();
		manager->myDispatch = manager->myDevice.make_table();

		return manager;
	}

	VulkanManager::VulkanManager()
		: myFrameIndex(static_cast<std::uint64_t>(-1))
		, myResourceManager(*this)
	{
		PROFILE_SCOPE();
	}

	VulkanManager::~VulkanManager()
	{
		PROFILE_SCOPE();

		vkb::destroy_device(myDevice);
		vkb::destroy_instance(myInstance);
	}

	std::shared_ptr<Atrium::FrameGraphicsContext> VulkanManager::CreateFrameGraphicsContext()
	{
		return nullptr;
	}

	std::uint_least64_t VulkanManager::GetCurrentFrameIndex() const
	{
		return myFrameIndex;
	}

	void VulkanManager::MarkFrameStart()
	{
		PROFILE_SCOPE();
	}

	void VulkanManager::MarkFrameEnd()
	{
		PROFILE_SCOPE();

		myFrameIndex += 1;
	}

	void VulkanManager::WaitForIdle() const
	{

	}
}