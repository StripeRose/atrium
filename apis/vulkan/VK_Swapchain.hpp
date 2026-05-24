#pragma once

#include "Atrium_RenderTexture.hpp"
#include "Atrium_WindowManagement.hpp"

#include <vulkan/vulkan_raii.hpp>

#include <memory>

namespace Atrium::Vulkan
{
	class VulkanManager;
	class Swapchain : public Atrium::RenderTexture
	{
	public:
		static std::unique_ptr<Swapchain> Create(VulkanManager& aManager, Atrium::Window& aWindow);

		// Inherited via RenderTexture
		TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;
		void* GetNativeTexturePtr() const override;
		const RenderTextureDescriptor& GetDescriptor() const override;
		void* GetNativeDepthBufferPtr() const override;

	private:
		Swapchain() = default;

		//void FetchPresentationQueue(Device& aDevice);

		vk::raii::SurfaceKHR mySurface = nullptr;
	};
}
