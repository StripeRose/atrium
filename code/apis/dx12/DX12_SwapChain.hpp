// Filter "Resources"

#pragma once

#include "DX12_Enums.hpp"
#include "DX12_RenderTexture.hpp"

#include <Platform_WindowManagement.hpp>

namespace RoseGold::DirectX12
{
	class Device;
	class SwapChain : public RenderTarget
	{
	public:
		SwapChain(Device& aDevice, Core::Platform::Window& aWindow);

		void Invalidate();
		void UpdateResolution();
		void Present();

		unsigned int GetCurrentBufferIndex() const;
		unsigned int GetBufferCount() const;

		const DescriptorHeapHandle* GetColorView() const override;
		const DescriptorHeapHandle* GetDepthStencilView() const override;

		ID3D12Resource* GetColorResource() const override;
		ID3D12Resource* GetDepthResource() const override;

		bool IsSwapChain() const override { return true; }

		// Implementing RenderTexture
	public:
		const Core::Graphics::RenderTextureDescriptor& GetDescriptor() const override;
		void* GetNativeDepthBufferPtr() const override;

		// Implementing Texture
	public:
		void SetFilterMode(Core::Graphics::FilterMode aFilterMode) override;
		Core::Graphics::TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		Core::Graphics::FilterMode GetFilterMode() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;

		Core::Graphics::TextureWrapMode GetWrapModeU() const override;
		Core::Graphics::TextureWrapMode GetWrapModeV() const override;
		Core::Graphics::TextureWrapMode GetWrapModeW() const override;

		void SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode) override;
		void SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const override;
		void SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const override;
		void SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const override;

		void* GetNativeTexturePtr() const override;

	private:
		void CreateRenderTextureForWindow();
		void DoBufferResizing();
		void UpdateColorSpace();
		void GetBackBuffers(int aWidth, int aHeight);

		void OnDrawSurfaceResize(int aWidth, int aHeight);

		Core::Graphics::RenderTextureFormat GetRenderTextureFormat() const;

	private:
		class SwapChainBackBuffer : public DirectX12::RenderTexture
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
		Core::Platform::Window* myWindow;

		std::vector<std::shared_ptr<SwapChainBackBuffer>> myBackBuffers;
		std::optional<std::pair<int, int>> myDesiredResolution;
	};
}
