#include "stdafx.hpp"
#include "DX12_ResourceManager.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Device.hpp"
#include "DX12_Manager.hpp"

namespace RoseGold::DirectX12
{
	ResourceManager::ResourceManager(DirectX12API& aManager)
		: myManager(aManager)
	{
		SetupRootSignature();
	}

	std::shared_ptr<Core::RenderTexture> ResourceManager::CreateRenderTextureForWindow(Core::Window& aWindow)
	{
		ZoneScoped;

		const std::scoped_lock lock(mySwapChainMutex);
		std::shared_ptr<SwapChain>& swapChain = myDrawSurfaceSwapChain[&aWindow];
		swapChain.reset(new SwapChain(myManager.GetDevice(), myManager.GetCommandQueueManager().GetGraphicsQueue(), aWindow));
		swapChain->SetName(aWindow.GetTitle().c_str());

		aWindow.Closed.Connect(this, [&](Core::Window& aWindow) {
			myDrawSurfaceSwapChain.at(&aWindow)->Invalidate();
			myDrawSurfaceSwapChain.erase(&aWindow);
			});

		return swapChain;
	}

	std::shared_ptr<Core::GraphicsBuffer> ResourceManager::CreateGraphicsBuffer(Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride)
	{
		ZoneScoped;

		switch (aTarget)
		{
		case Core::GraphicsBuffer::Target::Vertex:
			return std::shared_ptr<Core::GraphicsBuffer>(new VertexBuffer(myManager.GetDevice(), aCount, aStride));
		case Core::GraphicsBuffer::Target::Index:
			return std::shared_ptr<Core::GraphicsBuffer>(new IndexBuffer(myManager.GetDevice(), aCount));
		case Core::GraphicsBuffer::Target::Constant:
			return std::shared_ptr<Core::GraphicsBuffer>(new ConstantBuffer(myManager.GetDevice(), aCount * aStride));
		default:
			return nullptr;
		}
	}

	std::shared_ptr<Core::PipelineState> ResourceManager::CreateOrGetPipelineState(const Core::PipelineStateDescription& aPipelineState)
	{
		return DirectX12::PipelineState::CreateFrom(
			*myManager.GetDevice().GetDevice().Get(),
			myDefaultRootSignature,
			aPipelineState);
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

	std::shared_ptr<Core::Texture2D> ResourceManager::CreateTexture2D(unsigned int aWidth, unsigned int aHeight, Core::TextureFormat aTextureFormat)
	{
		return CreateDDS2D(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aHeight, aTextureFormat);
	}

	std::shared_ptr<Core::Texture3D> ResourceManager::CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat)
	{
		return CreateDDS3D(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aHeight, aDepth, aTextureFormat);
	}

	std::shared_ptr<Core::TextureCube> ResourceManager::CreateTextureCube(unsigned int aWidth, Core::TextureFormat aTextureFormat)
	{
		return CreateDDSCube(myManager.GetDevice(), myManager.GetUploadContext(), aWidth, aTextureFormat);
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
			swapChain.second->UpdateResolution();
	}

	void ResourceManager::SetupRootSignature()
	{
		ZoneScoped;

		RootSignatureCreator signature;

		signature.SetVisibility(D3D12_SHADER_VISIBILITY_VERTEX);
		{
			signature.AddDescriptorTable()
				.AddCBVRange(1, 0, Core::ResourceUpdateFrequency::PerObject) // Model, View, Projection data.
				;
		}

		signature.SetVisibility(D3D12_SHADER_VISIBILITY_PIXEL);
		{
			signature.AddDescriptorTable()
				.AddSRVRange(4, 0, Core::ResourceUpdateFrequency::PerMaterial)
				;

			signature.AddSampler(0) // Clamping Point
				.Filter(D3D12_FILTER_MIN_MAG_MIP_POINT)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
				;
			signature.AddSampler(1) // Clamping Linear
				.Filter(D3D12_FILTER_MIN_MAG_MIP_LINEAR)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
				;
			signature.AddSampler(2) // Clamping Anisotropic
				.Filter(D3D12_FILTER_ANISOTROPIC)
				.Address(D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
				;
		}

		myDefaultRootSignature = signature.Finalize(myManager.GetDevice().GetDevice().Get());
	}
}
