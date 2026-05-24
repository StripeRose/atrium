#pragma once

#include "Atrium_GraphicsAPI.hpp"

#include "VK_ResourceManager.hpp"

#include <rose-common/SemanticVersion.hpp>

#include <VkBootstrap.h>

namespace Atrium::Vulkan
{
	class VulkanManager final : public Atrium::GraphicsAPI
	{
		friend Vulkan::ResourceManager;
	public:
		static std::unique_ptr<VulkanManager> Create(std::string_view anApplicationName, SemanticVersion anApplicationVersion);

	private:
		VulkanManager();

	public:
		~VulkanManager();

		// Implementing Atrium::GraphicsAPI
	public:
		std::shared_ptr<Atrium::FrameGraphicsContext> CreateFrameGraphicsContext() override;

		std::uint_least64_t GetCurrentFrameIndex() const override;

		GraphicsAPI::ResourceManager& GetResourceManager() override { return myResourceManager; }

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

		void WaitForIdle() const override;

	private:
		vkb::Instance myInstance;
		vkb::InstanceDispatchTable myInstanceDispatch;
		vkb::PhysicalDevice myPhysicalDevice;
		vkb::Device myDevice;
		vkb::DispatchTable myDispatch;

		std::uint_least64_t myFrameIndex;

		Vulkan::ResourceManager myResourceManager;
	};
}