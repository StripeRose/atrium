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

	class DirectX12API final : public Core::GraphicsAPI
	{
	public:
		static std::size_t GetFramesInFlightAmount();

	public:
		DirectX12API();
		~DirectX12API();

		CommandQueueManager& GetCommandQueueManager() { return *myCommandQueueManager.get(); }

		Core::FrameGraphicsContext& GetCurrentFrameContext() override;

		Device& GetDevice() { return *myDevice; }

		Core::GraphicsAPI::ResourceManager& GetResourceManager() override { return *myResourceManager; }

		UploadContext& GetUploadContext() { return *myUploadContext; }

		bool SupportsMultipleWindows() const override { return true; }

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;

		std::unique_ptr<CommandQueueManager> myCommandQueueManager;

		std::unique_ptr<FrameGraphicsContext> myFrameGraphicsContext;
		std::unique_ptr<UploadContext> myUploadContext;

		struct FrameEndFences
		{
			std::uint64_t ComputeQueue;
			std::uint64_t CopyQueue;
			std::uint64_t GraphicsQueue;
		};

		std::vector<FrameEndFences> myFrameEndFences;

		std::uint64_t myFrameIndex;
		std::uint_least8_t myFrameInFlight;

		std::unique_ptr<DirectX12::ResourceManager> myResourceManager;
	};
}