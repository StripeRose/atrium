#include "DX12_ResourceManager.hpp"

#include "DX12_Device.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Manager.hpp"
#include "DX12_Texture.hpp"

namespace Atrium::DirectX12
{
	ResourceManager::ResourceManager(DirectX12API& aManager)
		: myManager(aManager)
	{
	}

	std::shared_ptr<Atrium::RenderTexture> ResourceManager::CreateRenderTextureForWindow(Window& aWindow)
	{
		PROFILE_SCOPE();

		const std::scoped_lock lock(mySwapChainMutex);
		std::shared_ptr<SwapChain>& swapChain = myDrawSurfaceSwapChain[&aWindow];
		swapChain.reset(new SwapChain(myManager.GetDevice(), myManager.GetCommandQueueManager().GetGraphicsQueue(), aWindow));
		swapChain->SetName(aWindow.GetTitle().c_str());

		aWindow.OnClosed.Connect(this, [&]() {
			myDrawSurfaceSwapChain.erase(&aWindow);
			});

		return swapChain;
	}

	std::shared_ptr<Atrium::GraphicsBuffer> ResourceManager::CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		PROFILE_SCOPE();

		return std::shared_ptr<Atrium::GraphicsBuffer>(new GraphicsBuffer(myManager, aTarget, aCount, aStride));
	}

	std::shared_ptr<Atrium::PipelineState> ResourceManager::CreatePipelineState(const PipelineStateDescription& aPipelineState)
	{
		return DirectX12::PipelineState::CreateFrom(
			*myManager.GetDevice().GetDevice().Get(),
			aPipelineState);
	}

	std::unique_ptr<Atrium::RootSignatureBuilder> ResourceManager::CreateRootSignature()
	{
		return std::unique_ptr<Atrium::RootSignatureBuilder>(new RootSignatureCreator(myManager.GetDevice().GetDevice().Get()));
	}

	std::shared_ptr<Atrium::Shader> ResourceManager::CreateShader(const std::filesystem::path& aSource, Atrium::Shader::Type aType, const char* anEntryPoint)
	{
		PROFILE_SCOPE();

		switch (aType)
		{
			case Atrium::Shader::Type::Vertex:
				return Shader::CreateFromSource(aSource, anEntryPoint, "vs_5_1");

			case Atrium::Shader::Type::Pixel:
				return Shader::CreateFromSource(aSource, anEntryPoint, "ps_5_1");

			default:
				return nullptr;
		}
	}

	std::shared_ptr<Atrium::Texture> ResourceManager::CreateTexture(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, unsigned int anArrayCount, TextureFormat aTextureFormat, std::optional<TextureDimension> aDimension)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = anArrayCount;
		metadata.depth = aDepth;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		// Fallback to simple deduction of dimension based on size.
		if (!aDimension)
		{
			if (aDepth > 1)
				aDimension = TextureDimension::Tex3D;
			else
				aDimension = TextureDimension::Tex2D;
		}

		switch (aDimension.value())
		{
			case TextureDimension::Cube:
				if (anArrayCount > 1)
					Debug::LogWarning("Texture dimension explicitly set to Cube but array count was greater than 1. Implicitly changed to CubeArray instead, but may be unintentional.");

				[[fallthrough]];

			case TextureDimension::CubeArray:
				// Ignoring aDepth and forcing 6 sides.
				metadata.depth = 6;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
				metadata.miscFlags = DirectX::TEX_MISC_TEXTURECUBE;
				break;

			case TextureDimension::Tex2D:
				// Forces 1 in depth for 2D textures.
				metadata.depth = 1;
				// To Atrium always 2D, but if height or width is 1 it's 1D to DirectX.
				metadata.dimension = aWidth > 1 && aHeight > 1 ? DirectX::TEX_DIMENSION_TEXTURE2D : DirectX::TEX_DIMENSION_TEXTURE1D;
				break;

			case TextureDimension::Tex3D:
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE3D;
				break;

			default:
				throw std::logic_error("Tried to create a texture with an invalid dimension.");
		}

		return std::make_shared<DirectX12::Texture>(myManager.GetDevice(), myManager.GetUploadContext(), metadata);
	}

	std::shared_ptr<SwapChain> ResourceManager::GetSwapChain(Window& aWindow)
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

	std::shared_ptr<Atrium::Texture> ResourceManager::LoadTexture(const std::filesystem::path& aPath)
	{
		PROFILE_SCOPE();

		const std::filesystem::path extension = aPath.extension();
		if (extension == ".dds")
		{
			std::unique_ptr<DirectX::ScratchImage> image = std::make_unique<DirectX::ScratchImage>();

			HRESULT loadResult = DirectX::LoadFromDDSFile(aPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *image);

			if (!Debug::Verify(loadResult, "Load DDS file from disk."))
				return nullptr;

			std::shared_ptr texture = std::make_shared<DirectX12::Texture>(myManager.GetDevice(), myManager.GetUploadContext(), std::move(image));
			texture->Apply(true, false);
			texture->GetImage().GetResource()->SetName(aPath.filename().c_str());

			return texture;
		}

		return nullptr;
	}
}
