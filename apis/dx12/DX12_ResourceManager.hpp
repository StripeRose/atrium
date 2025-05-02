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
	class ResourceManager : public Atrium::GraphicsAPI::ResourceManager
	{
	public:
		ResourceManager(DirectX12API& aManager);

		std::shared_ptr<Atrium::RenderTexture> CreateRenderTextureForWindow(Atrium::Window& aWindow) override;

		std::shared_ptr<Atrium::GraphicsBuffer> CreateGraphicsBuffer(GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) override;

		std::shared_ptr<Atrium::PipelineState> CreatePipelineState(const PipelineStateDescription& aPipelineState) override;

		std::unique_ptr<RootSignatureBuilder> CreateRootSignature() override;

		std::shared_ptr<Atrium::Shader> CreateShader(const std::filesystem::path& aSource, Shader::Type aType, const char* anEntryPoint) override;

		std::shared_ptr<Atrium::Texture2D> CreateTexture2D(unsigned int aWidth, unsigned int aHeight, TextureFormat aTextureFormat) override;

		std::shared_ptr<Atrium::Texture3D> CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat) override;

		std::shared_ptr<Atrium::TextureCube> CreateTextureCube(unsigned int aWidth, TextureFormat aTextureFormat) override;

		std::shared_ptr<SwapChain> GetSwapChain(Atrium::Window& aWindow);

		std::vector<std::shared_ptr<SwapChain>> GetSwapChains();

		std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& aPath) override;

		void MarkFrameStart();

	private:
		DirectX12API& myManager;

		std::mutex mySwapChainMutex;
		std::map<Atrium::Window*, std::shared_ptr<SwapChain>> myDrawSurfaceSwapChain;
	};
}