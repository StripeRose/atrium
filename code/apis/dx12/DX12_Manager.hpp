#pragma once

#include "DX12_ComPtr.hpp"

#include <Graphics_Manager.hpp>

#include <d3d12.h>

#include <memory>

namespace RoseGold::DirectX12
{
	class Device;
	class Manager final : public Core::Graphics::Manager
	{
	public:
		Manager();
		~Manager();

		std::shared_ptr<Core::Graphics::RenderTexture> CreateRenderTextureForWindow(Core::Platform::Window& aWindow) override;
		void ExecuteCommandBuffer(const Core::Graphics::CommandBuffer& aCommandBuffer) override;
		void ExecuteTask(const Core::Graphics::GraphicsTask& aGraphicsTask) override;

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;
		ComPtr<ID3D12CommandAllocator> myCommandAllocator;
		std::vector<std::shared_ptr<Core::Graphics::RenderTexture>> myFrameTargets;
	};
}