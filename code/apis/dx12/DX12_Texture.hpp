// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_DescriptorHeap.hpp"
#include "DX12_Enums.hpp"
#include "DX12_GPUResource.hpp"

#include "Graphics_Texture2D.hpp"
#include "Graphics_Texture3D.hpp"
#include "Graphics_TextureCube.hpp"

#include <d3d12.h>
#include <DirectXTex.h>

#include <filesystem>

namespace RoseGold::DirectX12
{
	class Device;
	class UploadContext;
	class TextureBackend : public Core::Graphics::Texture, public GPUResource, public std::enable_shared_from_this<TextureBackend>
	{
	public:
		static TextureBackend* FromTexture(Core::Graphics::Texture& aTexture);
		static const TextureBackend* FromTexture(const Core::Graphics::Texture& aTexture);

	public:
		TextureBackend(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata);
		TextureBackend(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage);

		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable);
		Core::Graphics::TextureFormat GetFormat() const;

		virtual DXGI_FORMAT GetDXGIFormat() const { return myImage->GetMetadata().format; }
		const std::shared_ptr<DescriptorHeapHandle>& GetSRVHandle() const { return mySRVHandle; }

		// Implements Texture
	public:

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

		void SetFilterMode(Core::Graphics::FilterMode aFilterMode) override;
		void SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode) override;
		void SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const override;
		void SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const override;
		void SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const override;

		void* GetNativeTexturePtr() const override;

	private:
		void Apply_SetupResource();
		void Apply_BeginImageUpload();

	private:
		Device& myDevice;
		UploadContext& myUploader;

		std::unique_ptr<DirectX::ScratchImage> myImage;
		DirectX::TexMetadata myMetadata;

		std::shared_ptr<DescriptorHeapHandle> mySRVHandle;
	};

	class Texture2D : public Core::Graphics::Texture2D
	{
	public:
		Texture2D(std::shared_ptr<TextureBackend> aBackend)
			: myBackend(aBackend)
		{ }

		TextureBackend& GetBackend() { return *myBackend; }
		const TextureBackend& GetBackend() const { return *myBackend; }

		// Implements Texture
	public:
		Core::Graphics::TextureDimension GetDimensions() const override { return myBackend->GetDimensions(); }
		unsigned int GetDepth() const override { return myBackend->GetDepth(); }
		Core::Graphics::FilterMode GetFilterMode() const override { return myBackend->GetFilterMode(); }
		unsigned int GetHeight() const override { return myBackend->GetHeight(); }
		bool IsReadable() const override { return myBackend->IsReadable(); }
		unsigned int GetMipmapCount() const override { return myBackend->GetMipmapCount(); }
		unsigned int GetWidth() const override { return myBackend->GetWidth(); }

		Core::Graphics::TextureWrapMode GetWrapModeU() const override { return myBackend->GetWrapModeU(); }
		Core::Graphics::TextureWrapMode GetWrapModeV() const override { return myBackend->GetWrapModeV(); }
		Core::Graphics::TextureWrapMode GetWrapModeW() const override { return myBackend->GetWrapModeW(); }

		void SetFilterMode(Core::Graphics::FilterMode aFilterMode) override { myBackend->SetFilterMode(aFilterMode); }
		void SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode) override { myBackend->SetWrapMode(aWrapMode); }
		void SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeU(aWrapMode); }
		void SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeV(aWrapMode); }
		void SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeW(aWrapMode); }

		void* GetNativeTexturePtr() const override { return myBackend->GetNativeTexturePtr(); }

		// Implements Texture2D
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override { myBackend->Apply(anUpdateMipmaps, aMakeNoLongerReadable); }
		Core::Graphics::TextureFormat GetFormat() const override { return myBackend->GetFormat(); }

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel) const override;
		Color GetPixelBilinear(float aU, float aV, unsigned int aMipLevel) const override;
		std::vector<Color> GetPixels(unsigned int aMipLevel) const override;
		std::vector<Color> GetPixels(unsigned int anX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, unsigned int aMipLevel) const override;

		void SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel) override;
		void SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel) const override;
		void SetPixels(unsigned int anX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, const std::vector<Color>& someColors, unsigned int aMipLevel) const override;

	private:
		std::shared_ptr<TextureBackend> myBackend;
	};

	class Texture3D : public Core::Graphics::Texture3D
	{
	public:
		Texture3D(std::shared_ptr<TextureBackend> aBackend)
			: myBackend(aBackend)
		{ }

		TextureBackend& GetBackend() { return *myBackend; }
		const TextureBackend& GetBackend() const { return *myBackend; }

		// Implements Texture
	public:
		Core::Graphics::TextureDimension GetDimensions() const override { return myBackend->GetDimensions(); }
		unsigned int GetDepth() const override { return myBackend->GetDepth(); }
		Core::Graphics::FilterMode GetFilterMode() const override { return myBackend->GetFilterMode(); }
		unsigned int GetHeight() const override { return myBackend->GetHeight(); }
		bool IsReadable() const override { return myBackend->IsReadable(); }
		unsigned int GetMipmapCount() const override { return myBackend->GetMipmapCount(); }
		unsigned int GetWidth() const override { return myBackend->GetWidth(); }

		Core::Graphics::TextureWrapMode GetWrapModeU() const override { return myBackend->GetWrapModeU(); }
		Core::Graphics::TextureWrapMode GetWrapModeV() const override { return myBackend->GetWrapModeV(); }
		Core::Graphics::TextureWrapMode GetWrapModeW() const override { return myBackend->GetWrapModeW(); }

		void SetFilterMode(Core::Graphics::FilterMode aFilterMode) override { myBackend->SetFilterMode(aFilterMode); }
		void SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode) override { myBackend->SetWrapMode(aWrapMode); }
		void SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeU(aWrapMode); }
		void SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeV(aWrapMode); }
		void SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeW(aWrapMode); }

		void* GetNativeTexturePtr() const override { return myBackend->GetNativeTexturePtr(); }

		// Implements Texture3D
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override { myBackend->Apply(anUpdateMipmaps, aMakeNoLongerReadable); }
		Core::Graphics::TextureFormat GetFormat() const override { return myBackend->GetFormat(); }

		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel) const override;
		Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel) const override;
		std::vector<Color> GetPixels(unsigned int aMipLevel) const override;

		void SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel) override;
		void SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel) const override;

	private:
		std::shared_ptr<TextureBackend> myBackend;
	};

	class TextureCube : public Core::Graphics::TextureCube
	{
	public:
		TextureCube(std::shared_ptr<TextureBackend> aBackend)
			: myBackend(aBackend)
		{ }

		TextureBackend& GetBackend() { return *myBackend; }
		const TextureBackend& GetBackend() const { return *myBackend; }

		// Implements Texture
	public:
		Core::Graphics::TextureDimension GetDimensions() const override { return myBackend->GetDimensions(); }
		unsigned int GetDepth() const override { return myBackend->GetDepth(); }
		Core::Graphics::FilterMode GetFilterMode() const override { return myBackend->GetFilterMode(); }
		unsigned int GetHeight() const override { return myBackend->GetHeight(); }
		bool IsReadable() const override { return myBackend->IsReadable(); }
		unsigned int GetMipmapCount() const override { return myBackend->GetMipmapCount(); }
		unsigned int GetWidth() const override { return myBackend->GetWidth(); }

		Core::Graphics::TextureWrapMode GetWrapModeU() const override { return myBackend->GetWrapModeU(); }
		Core::Graphics::TextureWrapMode GetWrapModeV() const override { return myBackend->GetWrapModeV(); }
		Core::Graphics::TextureWrapMode GetWrapModeW() const override { return myBackend->GetWrapModeW(); }

		void SetFilterMode(Core::Graphics::FilterMode aFilterMode) override { myBackend->SetFilterMode(aFilterMode); }
		void SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode) override { myBackend->SetWrapMode(aWrapMode); }
		void SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeU(aWrapMode); }
		void SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeV(aWrapMode); }
		void SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const override { myBackend->SetWrapModeW(aWrapMode); }

		void* GetNativeTexturePtr() const override { return myBackend->GetNativeTexturePtr(); }

		// Implements TextureCube
	public:
		void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) override { myBackend->Apply(anUpdateMipmaps, aMakeNoLongerReadable); }
		Core::Graphics::TextureFormat GetFormat() const override { return myBackend->GetFormat(); }

		Color GetPixel(Face aFace, unsigned int anX, unsigned int aY) const override;
		Color GetPixels(Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel) const override;

		void SetPixel(Face aFace, unsigned int anX, unsigned int aY, const Color& aColor) override;
		void SetPixels(const std::vector<Color>& someColors, Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel) override;

	private:
		std::shared_ptr<TextureBackend> myBackend;
	};
}
