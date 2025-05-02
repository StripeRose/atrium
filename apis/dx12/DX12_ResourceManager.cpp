#include "DX12_ResourceManager.hpp"

#include "DX12_Device.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Manager.hpp"
#include "DX12_Texture.hpp"

namespace Atrium::DirectX12
{
	ResourceManager::ResourceManager(DirectX12API& aManager)
		: myManager(aManager)
	{ }

	std::shared_ptr<Atrium::RenderTexture> ResourceManager::CreateRenderTextureForWindow(Atrium::Window& aWindow)
	{
		ZoneScoped;

		const std::scoped_lock lock(mySwapChainMutex);
		std::shared_ptr<SwapChain>& swapChain = myDrawSurfaceSwapChain[&aWindow];
		swapChain.reset(new SwapChain(myManager.GetDevice(), myManager.GetCommandQueueManager().GetGraphicsQueue(), aWindow));
		swapChain->SetName(aWindow.GetTitle().c_str());

		aWindow.Closed.Connect(this, [&](Atrium::Window& aWindow) {
			myDrawSurfaceSwapChain.erase(&aWindow);
			});

		return swapChain;
	}

	std::shared_ptr<Atrium::GraphicsBuffer> ResourceManager::CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		ZoneScoped;

		return std::shared_ptr<GraphicsBuffer>(new GraphicsBuffer(myManager, aTarget, aCount, aStride));
	}

	std::shared_ptr<Atrium::PipelineState> ResourceManager::CreatePipelineState(const PipelineStateDescription& aPipelineState)
	{
		return DirectX12::PipelineState::CreateFrom(
			*myManager.GetDevice().GetDevice().Get(),
			aPipelineState);
	}

	std::unique_ptr<RootSignatureBuilder> ResourceManager::CreateRootSignature()
	{
		return std::unique_ptr<RootSignatureBuilder>(new RootSignatureCreator(myManager.GetDevice().GetDevice().Get()));
	}

	std::shared_ptr<Atrium::Shader> ResourceManager::CreateShader(const std::filesystem::path& aSource, Shader::Type aType, const char* anEntryPoint)
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

	std::shared_ptr<Atrium::Texture2D> ResourceManager::CreateTexture2D(unsigned int aWidth, unsigned int aHeight, TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = 1;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		return std::make_shared<DirectX12::Texture2D>(myManager.GetDevice(), myManager.GetUploadContext(), metadata);
	}

	std::shared_ptr<Atrium::Texture3D> ResourceManager::CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = aDepth;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		return std::make_shared<DirectX12::Texture3D>(myManager.GetDevice(), myManager.GetUploadContext(), metadata);
	}

	std::shared_ptr<Atrium::TextureCube> ResourceManager::CreateTextureCube(unsigned int aWidth, TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = 6;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aWidth;
		metadata.mipLevels = 0;
		metadata.miscFlags = DirectX::TEX_MISC_TEXTURECUBE;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		return std::make_shared<DirectX12::TextureCube>(myManager.GetDevice(), myManager.GetUploadContext(), metadata);
	}

	std::shared_ptr<SwapChain> ResourceManager::GetSwapChain(Atrium::Window& aWindow)
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

	std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::filesystem::path& aPath)
	{
		ZoneScoped;

		const std::filesystem::path extension = aPath.extension();
		if (extension == ".dds")
		{
			std::unique_ptr<DirectX::ScratchImage> image = std::make_unique<DirectX::ScratchImage>();

			HRESULT loadResult = DirectX::LoadFromDDSFile(aPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *image);

			if (!Debug::Verify(loadResult, "Load DDS file from disk."))
				return nullptr;

			if (image->GetMetadata().arraySize != 1 && !image->GetMetadata().IsCubemap())
			{
				Debug::LogError("No support for DDS array textures.");
				return nullptr;
			}

			switch (image->GetMetadata().dimension)
			{
				case DirectX::TEX_DIMENSION_TEXTURE1D:
				case DirectX::TEX_DIMENSION_TEXTURE2D:
				{
					if (image->GetMetadata().IsCubemap())
					{
						std::shared_ptr texture = std::make_shared<DirectX12::TextureCube>(myManager.GetDevice(), myManager.GetUploadContext(), std::move(image));
						texture->Apply(true, false);
						texture->GetImage().GetResource()->SetName(aPath.filename().c_str());

						return texture;
					}
					else
					{
						std::shared_ptr texture = std::make_shared<DirectX12::Texture2D>(myManager.GetDevice(), myManager.GetUploadContext(), std::move(image));
						texture->Apply(true, false);
						texture->GetImage().GetResource()->SetName(aPath.filename().c_str());

						return texture;
					}
				}
				case DirectX::TEX_DIMENSION_TEXTURE3D:
				{
					std::shared_ptr texture = std::make_shared<DirectX12::Texture3D>(myManager.GetDevice(), myManager.GetUploadContext(), std::move(image));
					texture->Apply(true, false);
					texture->GetImage().GetResource()->SetName(aPath.filename().c_str());

					return texture;
				}
			}
		}

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
