// Filter "Resources"

#pragma once

#include "Graphics_Texture.hpp"

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_Enums.hpp"
#include "DX12_GPUResource.hpp"

#include <d3d12.h>
#include <DirectXTex.h>

#include <filesystem>

namespace RoseGold::DirectX12
{
	class Device;
	class UploadContext;
	class SimpleTexture : public Core::EditableTexture, public GPUResource, public std::enable_shared_from_this<SimpleTexture>
	{
	public:
		SimpleTexture(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		SimpleTexture(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override;

		Core::TextureFormat GetFormat() const override;

		virtual DXGI_FORMAT GetDXGIFormat() const { return myImage->GetMetadata().format; }
		const DescriptorHeapHandle& GetSRVHandle() const { return mySRVHandle; }

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel) const override;
		Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel) const override;

		void SetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, const Color& aColor, unsigned int aMipLevel) override;

		// Implements Texture
	public:

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

		void SetFilterMode(Core::FilterMode aFilterMode) override;
		void SetWrapMode(Core::TextureWrapMode aWrapMode) override;
		void SetWrapModeU(Core::TextureWrapMode aWrapMode) const override;
		void SetWrapModeV(Core::TextureWrapMode aWrapMode) const override;
		void SetWrapModeW(Core::TextureWrapMode aWrapMode) const override;

		void* GetNativeTexturePtr() const override;

	private:
		void Apply_SetupResource();
		void Apply_BeginImageUpload();

	private:
		Device& myDevice;
		UploadContext& myUploader;

		std::unique_ptr<DirectX::ScratchImage> myImage;
		DirectX::TexMetadata myMetadata;

		DescriptorHeapHandle mySRVHandle;
	};
}
