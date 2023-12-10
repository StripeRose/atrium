#pragma once

#include <Graphics_Manager.hpp>

#include <memory>

namespace RoseGold::DirectX12
{
	class Device;
	class Manager final : public Core::Graphics::Manager
	{
	public:
		Manager();
		~Manager();

		void CreateSwapChain(Core::Platform::Window& aWindow) override;

		bool SupportsMultipleWindows() const override { return true; }

	private:
		void ReportUnreleasedObjects();

	private:
		std::unique_ptr<Device> myDevice;
	};
}