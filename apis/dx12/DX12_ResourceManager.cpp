#include "DX12_ResourceManager.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Device.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Manager.hpp"

namespace Atrium::DirectX12
{
	ResourceManager::ResourceManager(DirectX12API& aManager)
		: myManager(aManager)
	{ }

	std::shared_ptr<Core::RenderTexture> ResourceManager::CreateRenderTextureForWindow(Core::Window& aWindow)
	{
		ZoneScoped;

		const std::scoped_lock lock(mySwapChainMutex);
		std::shared_ptr<SwapChain>& swapChain = myDrawSurfaceSwapChain[&aWindow];
		swapChain.reset(new SwapChain(myManager.GetDevice(), myManager.GetCommandQueueManager().GetGraphicsQueue(), aWindow));
		swapChain->SetName(aWindow.GetTitle().c_str());

		aWindow.Closed.Connect(this, [&](Core::Window& aWindow) {
			myDrawSurfaceSwapChain.erase(&aWindow);
			});

		return swapChain;
	}

	std::shared_ptr<Core::GraphicsBuffer> ResourceManager::CreateGraphicsBuffer(Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		ZoneScoped;

		return std::shared_ptr<Core::GraphicsBuffer>(new GraphicsBuffer(myManager, aTarget, aCount, aStride));
	}

	std::shared_ptr<Core::PipelineState> ResourceManager::CreatePipelineState(const Core::PipelineStateDescription& aPipelineState)
	{
		return DirectX12::PipelineState::CreateFrom(
			*myManager.GetDevice().GetDevice().Get(),
			aPipelineState);
	}

	std::unique_ptr<Core::RootSignatureBuilder> ResourceManager::CreateRootSignature()
	{
		return std::unique_ptr<Core::RootSignatureBuilder>(new RootSignatureCreator(myManager.GetDevice().GetDevice().Get()));
	}

	std::shared_ptr<Core::Shader> ResourceManager::CreateShader(const std::filesystem::path& aSource, Core::Shader::Type aType, const char* anEntryPoint)
	{
		ZoneScoped;

		switch (aType)
		{
		case Shader::Type::Vertex:
			return Shader::CreateFromSource(aSource, anEntryPoint, "vs_5_1");

		case Shader::Type::Pixel:
			return Shader::CreateFromSource(aSource, anEntryPoint, "ps_5_1");

		default:
			return nullptr;
		}
	}

	std::shared_ptr<Core::EditableTexture> ResourceManager::CreateTexture2D(unsigned int aWidth, unsigned int aHeight, Core::TextureFormat aTextureFormat)
	{
		return CreateEditableDDS(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aHeight, 1, aTextureFormat);
	}

	std::shared_ptr<Core::EditableTexture> ResourceManager::CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat)
	{
		return CreateEditableDDS(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aHeight, aDepth, aTextureFormat);
	}

	std::shared_ptr<Core::EditableTexture> ResourceManager::CreateTextureCube(unsigned int aWidth, Core::TextureFormat aTextureFormat)
	{
		return CreateEditableDDS(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aWidth, aWidth, aTextureFormat);
	}

	std::shared_ptr<SwapChain> ResourceManager::GetSwapChain(Core::Window& aWindow)
	{
		const std::scoped_lock lock(mySwapChainMutex);
		auto it = myDrawSurfaceSwapChain.find(&aWindow);
		return (it != myDrawSurfaceSwapChain.end()) ? it->second : nullptr;
	}

	std::vector<std::shared_ptr<SwapChain>> ResourceManager::GetSwapChains()
	{
		const std::scoped_lock lock(mySwapChainMutex);
		std::vector<std::shared_ptr<SwapChain>> swapChains;
		for (auto& swapChain : myDrawSurfaceSwapChain)
			swapChains.push_back(swapChain.second);
		return swapChains;
	}

	std::shared_ptr<Core::Texture> ResourceManager::LoadTexture(const std::filesystem::path& aPath)
	{
		const std::filesystem::path extension = aPath.extension();
		if (extension == ".dds")
			return LoadDDSFromFile(myManager.GetDevice(), myManager.GetUploadContext(), aPath);
		else
			return nullptr;
	}

	void ResourceManager::MarkFrameStart()
	{
		const std::scoped_lock lock(mySwapChainMutex);
		for (auto& swapChain : myDrawSurfaceSwapChain)
		{
			if (swapChain.second->NeedsResize())
			{
				myManager.GetCommandQueueManager().WaitForAllIdle();
				swapChain.second->TriggerResize();
			}
		}
	}
}
