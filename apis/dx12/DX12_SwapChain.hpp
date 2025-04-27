// Filter "Resources"

#pragma once

#include "DX12_Enums.hpp"
#include "DX12_RenderTexture.hpp"

#include "Atrium_WindowManagement.hpp"

namespace Atrium::DirectX12
{
	class CommandQueue;
	class Device;
	class SwapChain : public RenderTarget
	{
	public:
		SwapChain(Device& aDevice, CommandQueue& aDirectCommandQueue, Core::Window& aWindow);
		~SwapChain();

		void Invalidate();
		bool NeedsResize() const;
		void TriggerResize();
		void Present();

		unsigned int GetCurrentBufferIndex() const;
		unsigned int GetBufferCount() const;

		void SetName(const wchar_t* aName);

		// Implementing RenderTarget
	public:
		DescriptorHeapHandle GetColorView() const override;
		DescriptorHeapHandle GetDepthStencilView() const override;

		ID3D12Resource* GetColorResource() const override;
		ID3D12Resource* GetDepthResource() const override;

		std::shared_ptr<GPUResource> GetGPUResource() override;
		std::shared_ptr<GPUResource> GetDepthGPUResource() override;

		bool IsSwapChain() const override { return true; }

		// Implementing RenderTexture
	public:
		const Core::RenderTextureDescriptor& GetDescriptor() const override;
		void* GetNativeDepthBufferPtr() const override;

		// Implementing Texture
	public:
		void SetFilterMode(Core::FilterMode aFilterMode) override;
		Core::TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		Core::FilterMode GetFilterMode() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;

		Core::TextureWrapMode GetWrapModeU() const override;
		Core::TextureWrapMode GetWrapModeV() const override;
		Core::TextureWrapMode GetWrapModeW() const override;

		void SetWrapMode(Core::TextureWrapMode aWrapMode) override;
		void SetWrapModeU(Core::TextureWrapMode aWrapMode) const override;
		void SetWrapModeV(Core::TextureWrapMode aWrapMode) const override;
		void SetWrapModeW(Core::TextureWrapMode aWrapMode) const override;

		void* GetNativeTexturePtr() const override;

	private:
		void CreateRenderTextureForWindow(CommandQueue& aDirectCommandQueue);
		void DoBufferResizing();
		void UpdateColorSpace();
		void GetBackBuffers(const Size& aSize);

		void OnDrawSurfaceResize(const Size& aSize);

		Core::RenderTextureFormat GetRenderTextureFormat() const;

	private:
		class SwapChainBackBuffer : public DirectX12::RenderTexture
		{
		public:
			SwapChainBackBuffer(
				Device& aDevice,
				const Core::RenderTextureDescriptor& aDescriptor,
				const ComPtr<ID3D12Resource>& aColorBuffer = nullptr,
				const ComPtr<ID3D12Resource>& aDepthBuffer = nullptr);

			void Invalidate();
		};

	private:
		Device* myDevice;
		ComPtr<IDXGISwapChain3> mySwapChain;
		Core::Window* myWindow;

		std::vector<std::shared_ptr<SwapChainBackBuffer>> myBackBuffers;
		std::optional<Size> myDesiredResolution;
	};
}
