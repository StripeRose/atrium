#include "VK_Swapchain.hpp"

#include "VK_Manager.hpp"

namespace Atrium::Vulkan
{
	std::unique_ptr<Swapchain> Swapchain::Create(VulkanManager& aManager, Atrium::Window& aWindow)
	{
		aManager; aWindow;
		return nullptr;

		/*std::unique_ptr<Swapchain> createdSwapchain(new Swapchain());

		

		createdSwapchain->mySurface = std::move(surface.value());

		createdSwapchain->FetchPresentationQueue(aDevice);

		return createdSwapchain;*/
	}

	TextureDimension Swapchain::GetDimensions() const
	{
		return TextureDimension();
	}

	unsigned int Swapchain::GetDepth() const
	{
		return 0;
	}

	unsigned int Swapchain::GetHeight() const
	{
		return 0;
	}

	bool Swapchain::IsReadable() const
	{
		return false;
	}

	unsigned int Swapchain::GetMipmapCount() const
	{
		return 0;
	}

	unsigned int Swapchain::GetWidth() const
	{
		return 0;
	}

	void* Swapchain::GetNativeTexturePtr() const
	{
		return nullptr;
	}

	const RenderTextureDescriptor& Swapchain::GetDescriptor() const
	{
		static RenderTextureDescriptor c;
		return c;
	}

	void* Swapchain::GetNativeDepthBufferPtr() const
	{
		return nullptr;
	}

	/*void Swapchain::FetchPresentationQueue(Device& aDevice)
	{
		aDevice.GetPresentationQueueForSurface(mySurface);
	}*/
}
