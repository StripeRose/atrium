#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_FrameContext.hpp"

#include <Graphics_Manager.hpp>

#include <d3d12.h>

#include <memory>

namespace RoseGold::DirectX12
{
	class Device;
	class RootSignature;

	class Manager final : public Core::Graphics::Manager
	{
	public:
		Manager();
		~Manager();

		std::shared_ptr<Core::Graphics::RenderTexture> CreateRenderTextureForWindow(Core::Platform::Window& aWindow) override;

		std::shared_ptr<Core::Graphics::GraphicsBuffer> CreateGraphicsBuffer(Core::Graphics::GraphicsBuffer::Target aTarget, std::uint32_t aCount, std::uint32_t aStride) override;

		std::shared_ptr<Core::Graphics::PipelineState> CreateOrGetPipelineState(const Core::Graphics::PipelineStateDescription& aPipelineState) override;

		std::shared_ptr<Core::Graphics::Shader> CreateShader(const std::filesystem::path& aSource, Core::Graphics::Shader::Type aType, const char* anEntryPoint) override;

		void ExecuteCommandBuffer(const Core::Graphics::CommandBuffer& aCommandBuffer) override;
		void ExecuteTask(const Core::Graphics::GraphicsTask& aGraphicsTask) override;

		virtual std::shared_ptr<Core::Graphics::Texture> LoadTexture(const std::filesystem::path& aPath) override;

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		void SetupRootSignature();

		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;
		std::unique_ptr<FrameGraphicsContext> myFrameGraphicsContext;

		std::vector<std::shared_ptr<Core::Graphics::RenderTexture>> myFrameTargets;

		std::shared_ptr<RootSignature> myDefaultRootSignature;
	};
}