#include "VK_ResourceManager.hpp"

#include "VK_Device.hpp"
#include "VK_Manager.hpp"

namespace Atrium::Vulkan
{
	std::shared_ptr<Atrium::RenderTexture> ResourceManager::CreateRenderTextureForWindow(Window& aWindow)
	{
		aWindow;
		return nullptr;

		//PROFILE_SCOPE();

		//std::shared_ptr<Swapchain> createdSurface;
		//const std::scoped_lock lock(mySwapChainMutex);

		//Debug::Assert(mySwapchains[&aWindow].expired(), "Assuming no swap-chain exists for the window.");

		//std::any surfaceHandle = aWindow.GetNativeHandle();
		//HWND windowHandle = std::any_cast<HWND>(surfaceHandle);

		//vk::Win32SurfaceCreateInfoKHR createInfo{
		//	.hinstance = GetModuleHandle(nullptr),
		//	.hwnd = windowHandle
		//};

		////VkSurfaceKHR surface;

		////VkResult result = myManager.myInstanceDispatch.createWin32SurfaceKHR(createInfo, nullptr, &surface);
		///*if (!surface)
		//	return nullptr;*/

		////createdSurface.reset(Swapchain::Create(myManager, aWindow).release());
		//mySwapchains[&aWindow] = createdSurface;

		//return createdSurface;
	}

	std::shared_ptr<Atrium::GraphicsBuffer> ResourceManager::CreateGraphicsBuffer(Atrium::GraphicsBuffer::Target, std::uint32_t, std::uint32_t)
	{
		return nullptr;
	}

	std::shared_ptr<Atrium::PipelineState> ResourceManager::CreatePipelineState(const PipelineStateDescription&)
	{
		return nullptr;
	}

	std::unique_ptr<Atrium::RootSignatureBuilder> ResourceManager::CreateRootSignature()
	{
		return nullptr;
	}

	std::shared_ptr<Atrium::Shader> ResourceManager::CreateShader(const std::filesystem::path&, Atrium::Shader::Type, const char*)
	{
		return nullptr;
	}

	std::shared_ptr<Atrium::Texture> ResourceManager::CreateTexture(unsigned int, unsigned int, unsigned int, unsigned int, TextureFormat, std::optional<TextureDimension>)
	{
		return nullptr;
	}

	std::shared_ptr<Atrium::Texture> ResourceManager::LoadTexture(const std::filesystem::path&)
	{
		return nullptr;
	}
}