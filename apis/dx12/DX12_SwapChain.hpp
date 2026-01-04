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
		SwapChain(Device& aDevice, CommandQueue& aDirectCommandQueue, Atrium::Window& aWindow);
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
		const RenderTextureDescriptor& GetDescriptor() const override;
		void* GetNativeDepthBufferPtr() const override;

		// Implementing Texture
	public:
		TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;

		void* GetNativeTexturePtr() const override;

	private:
		void CreateRenderTextureForWindow(CommandQueue& aDirectCommandQueue);
		void DoBufferResizing();
		void UpdateColorSpace();
		void GetBackBuffers(const Vector2<int>& aSize);

		void OnDrawSurfaceResize(const Vector2<int>& aSize);

		RenderTextureFormat GetRenderTextureFormat() const;

	private:
		class SwapChainBackBuffer : public DirectX12::RenderTexture
		{
		public:
			SwapChainBackBuffer(
				Device& aDevice,
				const RenderTextureDescriptor& aDescriptor,
				const ComPtr<ID3D12Resource>& aColorBuffer = nullptr,
				const ComPtr<ID3D12Resource>& aDepthBuffer = nullptr);

			void Invalidate();
		};

	private:
		Device* myDevice;
		ComPtr<IDXGISwapChain3> mySwapChain;
		Atrium::Window* myWindow;

		std::vector<std::shared_ptr<SwapChainBackBuffer>> myBackBuffers;
		std::optional<Vector2<int>> myDesiredResolution;
	};
}
