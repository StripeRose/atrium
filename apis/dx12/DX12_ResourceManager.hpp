#pragma once

#include "DX12_Pipeline.hpp"
#include "DX12_SwapChain.hpp"

#include "Core_GraphicsAPI.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace Atrium::DirectX12
{
	class DirectX12API;
	class ResourceManager : public Core::GraphicsAPI::ResourceManager
	{
	public:
		ResourceManager(DirectX12API& aManager);

		std::shared_ptr<Core::RenderTexture> CreateRenderTextureForWindow(Core::Window& aWindow) override;

		std::shared_ptr<Core::GraphicsBuffer> CreateGraphicsBuffer(Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) override;

		std::shared_ptr<Core::PipelineState> CreatePipelineState(const Core::PipelineStateDescription& aPipelineState) override;

		std::unique_ptr<Core::RootSignatureBuilder> CreateRootSignature() override;

		std::shared_ptr<Core::Shader> CreateShader(const std::filesystem::path& aSource, Core::Shader::Type aType, const char* anEntryPoint) override;

		std::shared_ptr<Core::Texture2D> CreateTexture2D(unsigned int aWidth, unsigned int aHeight, Core::TextureFormat aTextureFormat) override;

		std::shared_ptr<Core::Texture3D> CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat) override;

		std::shared_ptr<Core::TextureCube> CreateTextureCube(unsigned int aWidth, Core::TextureFormat aTextureFormat) override;

		std::shared_ptr<SwapChain> GetSwapChain(Core::Window& aWindow);

		std::vector<std::shared_ptr<SwapChain>> GetSwapChains();

		std::shared_ptr<Core::Texture> LoadTexture(const std::filesystem::path& aPath) override;

		void MarkFrameStart();

	private:
		DirectX12API& myManager;

		std::mutex mySwapChainMutex;
		std::map<Core::Window*, std::shared_ptr<SwapChain>> myDrawSurfaceSwapChain;
	};
}