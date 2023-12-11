// Filter "Resources"

#pragma once

#include "DX12_Enums.hpp"
#include "DX12_RenderTexture.hpp"

#include <Platform_WindowManagement.hpp>

namespace RoseGold::DirectX12
{
	class Device;
	class SwapChain
	{
	public:
		SwapChain(Device& aDevice, Core::Platform::Window& aWindow);
		~SwapChain();

		void UpdateResolution();
		void Present();

		RenderTexture* GetRenderTexture();

		unsigned int GetCurrentBufferIndex() const;
		unsigned int GetBufferCount() const;

	private:
		void CreateSwapChain();
		void DoBufferResizing();
		void UpdateColorSpace();
		void GetBackBuffers(int aWidth, int aHeight);

		void OnDrawSurfaceResize(int aWidth, int aHeight);

		Core::Graphics::RenderTextureFormat GetRenderTextureFormat() const;

	private:
		class SwapChainBackBuffer : public RenderTexture
		{
		public:
			SwapChainBackBuffer(
				Device& aDevice,
				const Core::Graphics::RenderTextureDescriptor& aDescriptor,
				ComPtr<ID3D12Resource> aColorBuffer = nullptr,
				ComPtr<ID3D12Resource> aDepthBuffer = nullptr);

			void Invalidate();
		};

	private:
		Device* myDevice;
		ComPtr<IDXGISwapChain3> mySwapChain;
		const Core::Platform::Window* myWindow;

		std::vector<std::shared_ptr<SwapChainBackBuffer>> myBackBuffers;
		std::optional<std::pair<int, int>> myDesiredResolution;
	};
}
