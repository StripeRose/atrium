// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_Device.hpp"
#include "DX12_GPUResource.hpp"

#include <Graphics_Texture.hpp>
#include <Graphics_Texture2D.hpp>
#include <Graphics_Texture3D.hpp>

#include <d3d12.h>
#include <DirectXTex.h>

#include <filesystem>

namespace RoseGold::DirectX12
{
	std::shared_ptr<Core::Graphics::Texture> LoadDDSTextureFromFile(Device& aDevice, const std::filesystem::path& aPath);

	class Texture2D_DDS : public Core::Graphics::Texture2D, public GPUResource
	{
		friend std::shared_ptr<Core::Graphics::Texture> LoadDDSTextureFromFile(Device& aDevice, const std::filesystem::path& aPath);

	private:
		Texture2D_DDS(Device& aDevice, std::unique_ptr<DirectX::ScratchImage>&& anImage);

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

		// Implements Texture2D
	public:
		void Apply(bool anUpdateMipmaps = true, bool aMakeNoLongerReadable = false) override;
		void Apply_SetupResource();
		void Apply_BeginImageUpload();

		Core::Graphics::TextureFormat GetFormat() const override;

	private:
		std::unique_ptr<DirectX::ScratchImage> myImage;
		DirectX::TexMetadata myMetadata;

		Device& myDevice;
		std::shared_ptr<DescriptorHeapHandle> mySRVHandle;
	};
}
