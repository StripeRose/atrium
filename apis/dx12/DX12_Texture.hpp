// Filter "Resources"

#pragma once

#include "Core_Texture.hpp"

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

	class DDSImage
	{
	public:
		DDSImage(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		DDSImage(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable);

		const DirectX::TexMetadata& GetMetadata() const { return myMetadata; }
		const DirectX::ScratchImage* GetImage() const { return myImage.get(); }
		std::shared_ptr<GPUResource> GetResource() const { return myResource; }
		const DescriptorHeapHandle& GetSRVHandle() const { return mySRVHandle; }

	private:
		void Apply_SetupResource();
		void Apply_BeginImageUpload();

		Device& myDevice;
		UploadContext& myUploader;

		std::unique_ptr<DirectX::ScratchImage> myImage;
		DirectX::TexMetadata myMetadata;

		std::shared_ptr<GPUResource> myResource;
		DescriptorHeapHandle mySRVHandle;
	};

	class Texture : public Core::Texture
	{
	public:
		Texture(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		Texture(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable);

		DDSImage& GetImage() { return myImage; }

		// Implements Core::Texture
	public:
		Core::TextureDimension GetDimensions() const override;
		unsigned int GetDepth() const override;
		unsigned int GetHeight() const override;
		bool IsReadable() const override;
		unsigned int GetMipmapCount() const override;
		unsigned int GetWidth() const override;
		void* GetNativeTexturePtr() const override;

	private:
		DDSImage myImage;
	};
}
