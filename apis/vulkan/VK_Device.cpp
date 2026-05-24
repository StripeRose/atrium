//#include "VK_Device.hpp"
//#include "VK_CommandQueue.hpp"
//
//#include "Atrium_Diagnostics.hpp"
//
//#include <span>
//
//namespace Atrium::Vulkan
//{
//	std::unique_ptr<Device> Device::Create(const vk::ApplicationInfo& someAppInfo)
//	{
//		Debug::Log("Creating Vulkan device..");
//		return nullptr;
//	}
//
//	Device::~Device()
//	{
//
//	}
//
//	vk::raii::Queue Device::GetPresentationQueueForSurface(vk::raii::SurfaceKHR& aSurface)
//	{
//		auto getPresentationQueue = [&](const vk::raii::PhysicalDevice& aDevice) -> std::optional<uint32_t>
//			{
//				auto queueFamilyProperties = aDevice.getQueueFamilyProperties();
//
//				for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
//				{
//					if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
//						&& aDevice.getSurfaceSupportKHR(i, aSurface))
//					{
//						// found a queue family that supports both graphics and present
//						return i;
//					}
//				}
//
//				return {};
//			};
//
//		vk::raii::PhysicalDevice physicalDevice = PickPhysicalDevice(
//			[&](const vk::raii::PhysicalDevice& aDevice) -> bool
//			{
//				return getPresentationQueue(aDevice).has_value();
//			});
//
//		std::shared_ptr<vk::raii::Device> logicalDevice = GetLogicalDeviceForPhysical(physicalDevice);
//
//		if (!logicalDevice)
//			return nullptr; // Add std::expected
//
//		return logicalDevice->getQueue(getPresentationQueue(physicalDevice).value(), 0);
//	}
//
//	vk::raii::PhysicalDevice Device::PickPhysicalDevice(std::function<bool(const vk::raii::PhysicalDevice&)> aValidityCallback)
//	{
//		auto physicalDevices = myInstance.enumeratePhysicalDevices();
//		if (!physicalDevices || physicalDevices.value().empty())
//		{
//			Debug::LogError("Unable to find GPUs with Vulkan support.");
//			return nullptr;
//		}
//
//		std::vector<vk::raii::PhysicalDevice> validDevices = physicalDevices.value();
//		std::erase_if(validDevices,
//			[&](const vk::raii::PhysicalDevice& dev)
//			{
//				return !PickPhysicalDevice_IsValid(dev) || !aValidityCallback(dev);
//			});
//
//		std::sort(validDevices.begin(), validDevices.end(),
//			[&](const vk::raii::PhysicalDevice& aDevA, const vk::raii::PhysicalDevice& aDevB)
//			{
//				return !PickPhysicalDevice_Priority(aDevA, aDevB);
//			});
//
//		if (validDevices.empty())
//		{
//			Debug::LogError("Unable to find GPUs with matching requirements.");
//			return nullptr;
//		}
//
//		auto deviceProperties = validDevices.front().getProperties();
//
//		Debug::Log("Best matching device: VID:%04X, PID:%04X - %s",
//			deviceProperties.vendorID,
//			deviceProperties.deviceID,
//			deviceProperties.deviceName.data());
//
//		return validDevices.front();
//	}
//
//	bool Device::PickPhysicalDevice_IsValid(const vk::raii::PhysicalDevice& aDevice)
//	{
//		auto deviceProperties = aDevice.getProperties();
//
//		Debug::Log("Vulkan PhysicalDevice: VID:%04X, PID:%04X - %s",
//			deviceProperties.vendorID,
//			deviceProperties.deviceID,
//			deviceProperties.deviceName.data());
//
//		if (deviceProperties.apiVersion < vk::ApiVersion13)
//		{
//			Debug::Log("No support for API 1.3 or above.");
//			return false;
//		}
//
//		auto availableDeviceExtensions = aDevice.enumerateDeviceExtensionProperties();
//		if (!availableDeviceExtensions)
//		{
//			Debug::Log("Unable to fetch device extensions.");
//			return false;
//		}
//
//		bool supportsAllRequiredExtensions = true;
//
//		for (const auto& requiredExtension : ourRequiredDeviceExtensions)
//		{
//			const bool isSupported = std::ranges::any_of(availableDeviceExtensions.value(),
//				[requiredExtension](auto const& availableDeviceExtension)
//				{
//					return strcmp(availableDeviceExtension.extensionName, requiredExtension) == 0;
//				});
//
//			if (!isSupported)
//			{
//				Debug::Log("Required device extension \"%s\" not supported.", requiredExtension);
//			}
//
//			supportsAllRequiredExtensions &= isSupported;
//		}
//
//		return supportsAllRequiredExtensions;
//	}
//
//	bool Device::PickPhysicalDevice_Priority(const vk::raii::PhysicalDevice& aDeviceA, const vk::raii::PhysicalDevice& aDeviceB)
//	{
//		auto devicePriority = [](vk::PhysicalDeviceType aType) -> int
//			{
//				switch (aType)
//				{
//					case vk::PhysicalDeviceType::eDiscreteGpu: return 5;
//					case vk::PhysicalDeviceType::eIntegratedGpu: return 4;
//					case vk::PhysicalDeviceType::eOther: return 3;
//					case vk::PhysicalDeviceType::eVirtualGpu: return 2;
//					case vk::PhysicalDeviceType::eCpu: return 1;
//				}
//
//				Debug::LogError("Device priority had an unknown type %i", aType);
//				return 0;
//			};
//
//		return devicePriority(aDeviceA.getProperties().deviceType) < devicePriority(aDeviceB.getProperties().deviceType);
//	}
//
//	std::shared_ptr<vk::raii::Device> Device::GetLogicalDeviceForPhysical(const vk::raii::PhysicalDevice& aPhysicalDevice)
//	{
//		auto existingDeviceIterator = myDevices.find(aPhysicalDevice);
//
//		if (existingDeviceIterator != myDevices.end())
//		{
//			std::shared_ptr<vk::raii::Device>& device = existingDeviceIterator->second;
//			return device;
//		}
//
//		const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = aPhysicalDevice.getQueueFamilyProperties();
//
//		std::optional<uint32_t> graphicsQueueIndex;
//		for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
//		{
//			const auto& qfp = queueFamilyProperties[i];
//
//			const bool supportGraphics = (qfp.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics;
//			if (!supportGraphics)
//				continue;
//
//			graphicsQueueIndex = i;
//		}
//
//		if (!graphicsQueueIndex)
//		{
//			Debug::LogError("Unable to find a queue family capable of graphics.");
//			return nullptr;
//		}
//
//		auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp)
//			{
//				return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
//			});
//		auto graphicsIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
//		const float queuePriority = 0.5f;
//		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
//			.queueFamilyIndex = graphicsIndex,
//			.queueCount = 1,
//			.pQueuePriorities = &queuePriority
//		};
//
//		vk::PhysicalDeviceFeatures deviceFeatures;
//
//		vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
//			{},
//			{.dynamicRendering = true },
//			{.extendedDynamicState = true }
//		};
//
//		vk::DeviceCreateInfo deviceCreateInfo
//		{
//			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
//			.queueCreateInfoCount = 1,
//			.pQueueCreateInfos = &deviceQueueCreateInfo,
//			.enabledExtensionCount = static_cast<uint32_t>(std::size(ourRequiredDeviceExtensions)),
//			.ppEnabledExtensionNames = ourRequiredDeviceExtensions
//		};
//
//		auto result = aPhysicalDevice.createDevice(deviceCreateInfo);
//		if (!result)
//		{
//			Debug::LogError("Unable to create logical device.");
//			return nullptr;
//		}
//
//		std::shared_ptr sharedDevice = std::make_shared<vk::raii::Device>(std::move(result.value()));
//
//		myDevices.insert({ aPhysicalDevice, sharedDevice });
//
//		// Todo: Move command queue to be per device.
//		myCommandQueueManager.reset(new CommandQueueManager(
//			sharedDevice->getQueue(graphicsIndex, 0)
//		));
//
//		return sharedDevice;
//	}
//
//	vk::raii::PhysicalDevice Device::GetPhysicalDeviceForSurface(vk::raii::SurfaceKHR& aSurface)
//	{
//		aSurface;
//		return nullptr;
//	}
//}