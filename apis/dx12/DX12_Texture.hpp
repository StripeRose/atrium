// Filter "Resources"

#pragma once

#include "Atrium_Texture.hpp"

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_Enums.hpp"
#include "DX12_GPUResource.hpp"

#include <d3d12.h>
#include <DirectXTex.h>

#include <filesystem>

namespace Atrium::DirectX12
{
	class Device;
	class UploadContext;
	class SimpleTexture : public Atrium::EditableTexture
	{
	public:
		SimpleTexture(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		SimpleTexture(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override;

		TextureFormat GetFormat() const override;

		virtual DXGI_FORMAT GetDXGIFormat() const { return myImage->GetMetadata().format; }
		const DescriptorHeapHandle& GetSRVHandle() const { return mySRVHandle; }

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel) const override;
		Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel) const override;

		std::shared_ptr<GPUResource> GetResource() { return myResource; }

		void SetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, const Color& aColor, unsigned int aMipLevel) override;

		// Implements Texture
	public:

		TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		FilterMode GetFilterMode() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;

		TextureWrapMode GetWrapModeU() const override;
		TextureWrapMode GetWrapModeV() const override;
		TextureWrapMode GetWrapModeW() const override;

		void SetFilterMode(FilterMode aFilterMode) override;
		void SetWrapMode(TextureWrapMode aWrapMode) override;
		void SetWrapModeU(TextureWrapMode aWrapMode) const override;
		void SetWrapModeV(TextureWrapMode aWrapMode) const override;
		void SetWrapModeW(TextureWrapMode aWrapMode) const override;

		void* GetNativeTexturePtr() const override;

	private:
		void Apply_SetupResource();
		void Apply_BeginImageUpload();

	private:
		Device& myDevice;
		UploadContext& myUploader;

		std::unique_ptr<DirectX::ScratchImage> myImage;
		DirectX::TexMetadata myMetadata;

		std::shared_ptr<GPUResource> myResource;
		DescriptorHeapHandle mySRVHandle;
	};
}
