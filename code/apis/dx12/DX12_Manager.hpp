#pragma once

#include "DX12_CommandQueue.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_FrameContext.hpp"
#include "DX12_SwapChain.hpp"

#include "Graphics_Manager.hpp"

#include "Platform_WindowManagement.hpp"

#include <d3d12.h>

#include <memory>

namespace RoseGold::DirectX12
{
	class Device;
	class RootSignature;

	class Manager final : public Core::Manager
	{
	public:
		Manager();
		~Manager();

		std::shared_ptr<Core::RenderTexture> CreateRenderTextureForWindow(Core::Window& aWindow) override;

		std::shared_ptr<Core::GraphicsBuffer> CreateGraphicsBuffer(Core::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) override;

		std::shared_ptr<Core::PipelineState> CreateOrGetPipelineState(const Core::PipelineStateDescription& aPipelineState) override;

		std::shared_ptr<Core::Shader> CreateShader(const std::filesystem::path& aSource, Core::Shader::Type aType, const char* anEntryPoint) override;

		std::shared_ptr<Core::Texture2D> CreateTexture2D(unsigned int aWidth, unsigned int aHeight, Core::TextureFormat aTextureFormat) override;
		std::shared_ptr<Core::Texture3D> CreateTexture3D(unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat) override;
		std::shared_ptr<Core::TextureCube> CreateTextureCube(unsigned int aWidth, Core::TextureFormat aTextureFormat) override;

		CommandQueueManager& GetCommandQueueManager() { return *myCommandQueueManager.get(); }

		Core::FrameContext& GetCurrentFrameContext() override;

		std::shared_ptr<SwapChain> GetSwapChain(Core::Window& aWindow);

		std::vector<std::shared_ptr<SwapChain>> GetSwapChains();

		virtual std::shared_ptr<Core::Texture> LoadTexture(const std::filesystem::path& aPath) override;

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		void SetupRootSignature();

		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;

		std::mutex mySwapChainMutex;
		std::map<Core::Window*, std::shared_ptr<SwapChain>> myDrawSurfaceSwapChain;

		std::unique_ptr<CommandQueueManager> myCommandQueueManager;

		std::unique_ptr<FrameGraphicsContext> myFrameGraphicsContext;
		std::unique_ptr<UploadContext> myUploadContext;

		std::array<std::uint64_t, DX12_FRAMES_IN_FLIGHT> myComputeQueueFrameEndFence;
		std::array<std::uint64_t, DX12_FRAMES_IN_FLIGHT> myCopyQueueFrameEndFence;
		std::array<std::uint64_t, DX12_FRAMES_IN_FLIGHT> myGraphicsQueueFrameEndFence;

		std::uint64_t myFrameIndex;
		std::uint_least8_t myFrameInFlight;

		std::shared_ptr<RootSignature> myDefaultRootSignature;
	};
}