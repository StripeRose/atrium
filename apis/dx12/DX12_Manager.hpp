#pragma once

#include "DX12_CommandQueue.hpp"
#include "DX12_ComPtr.hpp"
#include "DX12_FrameContext.hpp"
#include "DX12_ResourceManager.hpp"
#include "DX12_SwapChain.hpp"

#include "Core_GraphicsAPI.hpp"
#include "Core_WindowManagement.hpp"

#include <d3d12.h>

#include <memory>

namespace Atrium::DirectX12
{
	class Device;
	class RootSignature;

	class DirectX12API final : public Atrium::Core::GraphicsAPI
	{
	public:
		static std::size_t GetFramesInFlightAmount();

	public:
		DirectX12API();
		~DirectX12API();

		CommandQueueManager& GetCommandQueueManager() { return *myCommandQueueManager.get(); }

		Device& GetDevice() { return *myDevice; }

		std::uint_least8_t GetFrameInFlight() const { return myFrameInFlight; }

		UploadContext& GetUploadContext() { return *myUploadContext; }

		// Implementing Atrium::GraphicsAPI
	public:
		std::shared_ptr<Core::FrameGraphicsContext> CreateFrameGraphicsContext() override;

		std::unique_ptr<Core::GUIContext> CreateGUIContext(const std::shared_ptr<Core::RenderTexture>& aRenderTarget) override;

		std::uint_least64_t GetCurrentFrameIndex() const override;

		Core::GraphicsAPI::ResourceManager& GetResourceManager() override { return *myResourceManager; }

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;

		std::unique_ptr<CommandQueueManager> myCommandQueueManager;

		std::unique_ptr<FrameGraphicsContext> myPresentPrepareContext;
		std::vector<std::shared_ptr<FrameGraphicsContext>> myFrameGraphicsContexts;
		std::unique_ptr<UploadContext> myUploadContext;

		struct FrameEndFences
		{
			std::uint64_t ComputeQueue;
			std::uint64_t CopyQueue;
			std::uint64_t GraphicsQueue;
		};

		std::vector<FrameEndFences> myFrameEndFences;

		std::uint_least64_t myFrameIndex;
		std::uint_least8_t myFrameInFlight;

		std::unique_ptr<DirectX12::ResourceManager> myResourceManager;
	};
}