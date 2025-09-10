#pragma once

#include "DX12_Pipeline.hpp"
#include "DX12_SwapChain.hpp"

#include "Atrium_GraphicsAPI.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace Atrium::DirectX12
{
	class DirectX12API;
	class ResourceManager : public GraphicsAPI::ResourceManager
	{
	public:
		ResourceManager(DirectX12API& aManager);

		std::shared_ptr<Atrium::RenderTexture> CreateRenderTextureForWindow(Window& aWindow) override;

		std::shared_ptr<Atrium::GraphicsBuffer> CreateGraphicsBuffer(Atrium::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) override;

		std::shared_ptr<Atrium::PipelineState> CreatePipelineState(const PipelineStateDescription& aPipelineState) override;

		std::unique_ptr<Atrium::RootSignatureBuilder> CreateRootSignature() override;

		std::shared_ptr<Atrium::Shader> CreateShader(const std::filesystem::path& aSource, Atrium::Shader::Type aType, const char* anEntryPoint) override;

		std::shared_ptr<Atrium::Texture> CreateTexture(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, unsigned int anArrayCount, TextureFormat aTextureFormat, std::optional<TextureDimension> aDimension) override;

		std::shared_ptr<SwapChain> GetSwapChain(Window& aWindow);

		std::vector<std::shared_ptr<SwapChain>> GetSwapChains();

		std::shared_ptr<Atrium::Texture> LoadTexture(const std::filesystem::path& aPath) override;

		void MarkFrameStart();

	private:
		DirectX12API& myManager;

		std::mutex mySwapChainMutex;
		std::map<Window*, std::shared_ptr<SwapChain>> myDrawSurfaceSwapChain;
	};
}