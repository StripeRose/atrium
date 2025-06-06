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

	class Texture2D : public Core::Texture2D
	{
	public:
		Texture2D(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		Texture2D(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		DDSImage& GetImage() { return myTexture; }

		// Implements Texture2D
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override;

		Core::TextureFormat GetFormat() const override;

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const override;
		Color GetPixelBilinear(float aU, float aV, unsigned int aMipLevel = 0) const override;

		void SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel = 0) override;

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
		DDSImage myTexture;
	};

	class Texture3D : public Core::Texture3D
	{
	public:
		Texture3D(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		Texture3D(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		DDSImage& GetImage() { return myTexture; }

		// Implements Texture3D
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override;

		Core::TextureFormat GetFormat() const override;

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel = 0) const override;
		Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel = 0) const override;

		void SetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, const Color& aColor, unsigned int aMipLevel = 0) override;

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
		DDSImage myTexture;
	};

	class TextureCube : public Core::TextureCube
	{
	public:
		TextureCube(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		TextureCube(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		DDSImage& GetImage() { return myTexture; }

		// Implements TextureCube
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override;

		Core::TextureFormat GetFormat() const override;

		Color GetPixel(Core::TextureCubeFace aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const override;

		void SetPixel(Core::TextureCubeFace aFace, unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel = 0) override;

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
		DDSImage myTexture;
	};
}
