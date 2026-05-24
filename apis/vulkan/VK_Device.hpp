//#pragma once
//
//#include <vulkan/vulkan_raii.hpp>
//
//#include <functional>
//#include <map>
//#include <memory>
//#include <span>
//
//namespace Atrium::Vulkan
//{
//	class CommandQueueManager;
//	class Device
//	{
//	public:
//		static std::unique_ptr<Device> Create(const vk::ApplicationInfo& someAppInfo);
//
//		~Device();
//
//		//vk::raii::Device& GetDevice() { return myDevice; }
//		//vk::raii::PhysicalDevice& GetPhysicalDevice() { return myPhysicalDevice; }
//		CommandQueueManager& GetCommandQueueManager() { return *myCommandQueueManager; }
//
//		vk::raii::Queue GetPresentationQueueForSurface(vk::raii::SurfaceKHR& aSurface);
//
//	private:
//		vk::raii::PhysicalDevice PickPhysicalDevice(std::function<bool(const vk::raii::PhysicalDevice&)> aValidityCallback);
//		bool PickPhysicalDevice_IsValid(const vk::raii::PhysicalDevice& aDevice);
//		bool PickPhysicalDevice_Priority(const vk::raii::PhysicalDevice& aDeviceA, const vk::raii::PhysicalDevice& aDeviceB);
//
//		std::shared_ptr<vk::raii::Device> GetLogicalDeviceForPhysical(const vk::raii::PhysicalDevice& aDevice);
//
//		static bool CheckRequiredLayers(vk::raii::Context& aContext, std::span<const char*> someLayers);
//		static bool CheckRequiredExtensions(vk::raii::Context& aContext, std::span<const char*> someExtensions);
//
//		vk::raii::PhysicalDevice GetPhysicalDeviceForSurface(vk::raii::SurfaceKHR& aSurface);
//
//		std::map<vk::raii::PhysicalDevice, std::shared_ptr<vk::raii::Device>> myDevices;
//
//		std::unique_ptr<CommandQueueManager> myCommandQueueManager;
//	};
//}
