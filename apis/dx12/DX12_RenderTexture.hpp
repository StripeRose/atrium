// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_GPUResource.hpp"

#include "Atrium_RenderTexture.hpp"

namespace Atrium::DirectX12
{
	class RenderTarget : public Core::RenderTexture
	{
	public:
		virtual DescriptorHeapHandle GetColorView() const = 0;
		virtual DescriptorHeapHandle GetDepthStencilView() const = 0;

		virtual ID3D12Resource* GetColorResource() const = 0;
		virtual ID3D12Resource* GetDepthResource() const = 0;

		virtual std::shared_ptr<GPUResource> GetGPUResource() = 0;
		virtual std::shared_ptr<GPUResource> GetDepthGPUResource() = 0;

		virtual bool IsSwapChain() const = 0;
	};

	class Device;
	class RenderTexture : public RenderTarget
	{
	public:
		RenderTexture(
			Device& aDevice,
			const Core::RenderTextureDescriptor& aDescriptor
		);
		RenderTexture(
			Device& aDevice,
			const Core::RenderTextureDescriptor& aDescriptor,
			const std::shared_ptr<GPUResource>& aColorBuffer,
			const std::shared_ptr<GPUResource>& aDepthBuffer
		);
		~RenderTexture() override = default;

		// Implementing RenderTarget
	public:
		DescriptorHeapHandle GetColorView() const override { return myRSVHandle; }
		DescriptorHeapHandle GetDepthStencilView() const override { return myDSVHandle; }

		ID3D12Resource* GetColorResource() const override { return myResource->GetResource().Get(); }
		ID3D12Resource* GetDepthResource() const override { return myDepthResource->GetResource().Get(); }

		std::shared_ptr<GPUResource> GetGPUResource() override { return myResource; }
		std::shared_ptr<GPUResource> GetDepthGPUResource() override { return myDepthResource; }

		bool IsSwapChain() const override { return false; }

		void SetName(const wchar_t* aName);

		// Implementing RenderTexture
	public:
		const Core::RenderTextureDescriptor& GetDescriptor() const override { return myDescriptor; }
		void* GetNativeDepthBufferPtr() const override { return myDepthResource->GetResource().Get(); }

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

		void* GetNativeTexturePtr() const override { return myResource->GetResource().Get(); }

	protected:
		Device* myDevicePtr;

		Core::RenderTextureDescriptor myDescriptor;

		std::shared_ptr<GPUResource> myResource;
		std::shared_ptr<GPUResource> myDepthResource;

		DescriptorHeapHandle myRSVHandle;
		DescriptorHeapHandle myDSVHandle;
	};
}