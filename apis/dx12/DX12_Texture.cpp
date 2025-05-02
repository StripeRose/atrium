// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"
#include "DX12_Manager.hpp"
#include "DX12_MemoryAlignment.hpp"
#include "DX12_Texture.hpp"

namespace Atrium::DirectX12
{
	using namespace DirectX;

	DDSImage::DDSImage(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myDevice(aDevice)
		, myUploader(anUploader)
		, myImage(std::make_unique<DirectX::ScratchImage>())
		, myMetadata(aMetadata)
	{
		myImage->Initialize(aMetadata);
	}

	DDSImage::DDSImage(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myDevice(aDevice)
		, myUploader(anUploader)
		, myMetadata(anImage->GetMetadata())
	{
		std::swap(myImage, anImage);
	}

	void DDSImage::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
	{
		if (anUpdateMipmaps)
		{
			GenerateMipMaps(
				*myImage->GetImage(0, 0, 0),
				TEX_FILTER_DEFAULT,
				myMetadata.mipLevels,
				*myImage,
				myMetadata.dimension == TEX_DIMENSION_TEXTURE1D
			);
		}

		Apply_SetupResource();
		Apply_BeginImageUpload();

		if (aMakeNoLongerReadable)
			myImage.reset();
	}

	// Todo: Keep resource if it's still the right setup.
	void DDSImage::Apply_SetupResource()
	{
		D3D12_RESOURCE_DESC textureDesc;
		textureDesc.Format = myMetadata.format;
		textureDesc.Width = static_cast<UINT64>(myMetadata.width);
		textureDesc.Height = static_cast<UINT>(myMetadata.height);
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = static_cast<UINT16>(myMetadata.arraySize * myMetadata.depth);
		textureDesc.MipLevels = static_cast<UINT16>(myMetadata.mipLevels);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		switch (myMetadata.dimension)
		{
		case TEX_DIMENSION_TEXTURE1D:
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case TEX_DIMENSION_TEXTURE2D:
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case TEX_DIMENSION_TEXTURE3D:
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		}

		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Alignment = 0;

		D3D12_HEAP_PROPERTIES defaultProperties;
		defaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		defaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		defaultProperties.CreationNodeMask = 0;
		defaultProperties.VisibleNodeMask = 0;

		myResource = myDevice.CreateResource(
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr
		);

		mySRVHandle = myDevice.GetDescriptorHeapManager().GetShaderResourceViewHeap().GetNewHeapHandle();

		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDescPtr = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		if (myMetadata.IsCubemap())
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = static_cast<UINT>(myMetadata.mipLevels);
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			srvDescPtr = &srvDesc;
		}


		myDevice.GetDevice()->CreateShaderResourceView(
			myResource->GetResource().Get(),
			srvDescPtr,
			mySRVHandle.GetCPUHandle()
		);
	}

	void DDSImage::Apply_BeginImageUpload()
	{
		UploadContext::TextureUpload& textureUpload = myUploader.AddTextureUpload();
		textureUpload.Resource = myResource;
		textureUpload.SubresourceCount = static_cast<std::uint32_t>(myMetadata.mipLevels * myMetadata.arraySize);

		UINT numRows[UploadContext::MaxTextureSubresourceCount];
		std::uint64_t rowSizeInBytes[UploadContext::MaxTextureSubresourceCount];

		D3D12_RESOURCE_DESC resourceDesc = myResource->GetResource()->GetDesc();
		myDevice.GetDevice()->GetCopyableFootprints(
			&resourceDesc,
			0,
			textureUpload.SubresourceCount,
			0,
			textureUpload.SubresourceLayouts.data(),
			numRows,
			rowSizeInBytes,
			&textureUpload.BufferSize
		);

		textureUpload.BufferData.reset(new std::uint8_t[textureUpload.BufferSize]);

		for (uint64_t arrayIndex = 0; arrayIndex < myMetadata.arraySize; arrayIndex++)
		{
			for (uint64_t mipIndex = 0; mipIndex < myMetadata.mipLevels; mipIndex++)
			{
				const uint64_t subResourceIndex = mipIndex + (arrayIndex * myMetadata.mipLevels);

				const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = textureUpload.SubresourceLayouts[subResourceIndex];
				const uint64_t subResourceHeight = numRows[subResourceIndex];
				const uint64_t subResourcePitch = Align<UINT>(subResourceLayout.Footprint.RowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
				const uint64_t subResourceDepth = subResourceLayout.Footprint.Depth;
				uint8_t* destinationSubResourceMemory = &textureUpload.BufferData.get()[subResourceLayout.Offset];

				for (uint64_t sliceIndex = 0; sliceIndex < subResourceDepth; sliceIndex++)
				{
					const DirectX::Image* subImage = myImage->GetImage(mipIndex, arrayIndex, sliceIndex);
					const uint8_t* sourceSubResourceMemory = subImage->pixels;

					for (uint64_t height = 0; height < subResourceHeight; height++)
					{
						memcpy(destinationSubResourceMemory, sourceSubResourceMemory, (std::min)(subResourcePitch, subImage->rowPitch));
						destinationSubResourceMemory += subResourcePitch;
						sourceSubResourceMemory += subImage->rowPitch;
					}
				}
			}
		}
	}

	Texture2D::Texture2D(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myTexture(aDevice, anUploader, aMetadata)
	{
		Debug::Assert(
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE1D ||
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE2D
			, "Texture2D only support 1D or 2D DDS data.");
	}

	Texture2D::Texture2D(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myTexture(aDevice, anUploader, std::forward<std::unique_ptr<DirectX::ScratchImage>>(anImage))
	{
		Debug::Assert(
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE1D ||
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE2D
			, "Texture2D only support 1D or 2D DDS data.");
	}

	void Texture2D::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
	{
		myTexture.Apply(anUpdateMipmaps, aMakeNoLongerReadable);
	}

	TextureDimension Texture2D::GetDimensions() const
	{
		return TextureDimension::Tex2D;
	}

	unsigned int Texture2D::GetDepth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().depth);
	}

	FilterMode Texture2D::GetFilterMode() const
	{
		Debug::LogWarning("Texture2D::GetFilterMode() is not implemented. Defaulting to FilterMode::Point.");
		return FilterMode::Point;
	}

	TextureFormat Texture2D::GetFormat() const
	{
		return ToTextureFormat(myTexture.GetMetadata().format);
	}

	unsigned int Texture2D::GetHeight() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().height);
	}

	bool Texture2D::IsReadable() const
	{
		return myTexture.GetImage() != nullptr;
	}

	unsigned int Texture2D::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().mipLevels);
	}

	unsigned int Texture2D::GetWidth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().width);
	}

	TextureWrapMode Texture2D::GetWrapModeU() const
	{
		Debug::LogWarning("Texture2D::GetWrapModeU() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode Texture2D::GetWrapModeV() const
	{
		Debug::LogWarning("Texture2D::GetWrapModeV() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode Texture2D::GetWrapModeW() const
	{
		Debug::LogWarning("Texture2D::GetWrapModeW() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	void Texture2D::SetFilterMode(FilterMode aFilterMode)
	{
		aFilterMode;
		Debug::LogError("Texture2D::SetFilterMode() is not implemented. Call ignored.");
	}

	void Texture2D::SetWrapMode(TextureWrapMode aWrapMode)
	{
		aWrapMode;
		Debug::LogError("Texture2D::SetWrapMode() is not implemented. Call ignored.");
	}

	void Texture2D::SetWrapModeU(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture2D::SetWrapModeU() is not implemented. Call ignored.");
	}

	void Texture2D::SetWrapModeV(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture2D::SetWrapModeV() is not implemented. Call ignored.");
	}

	void Texture2D::SetWrapModeW(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture2D::SetWrapModeW() is not implemented. Call ignored.");
	}

	void* Texture2D::GetNativeTexturePtr() const
	{
		return myTexture.GetResource()->GetResource().Get();
	}

	Color Texture2D::GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel) const
	{
		anX; aY; aMipLevel;
		return Color::Predefined::Black;
	}

	Color Texture2D::GetPixelBilinear(float aU, float aV, unsigned int aMipLevel) const
	{
		aU; aV; aMipLevel;
		return Color::Predefined::Black;
	}

	void Texture2D::SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel)
	{
		anX; aY; aColor; aMipLevel;
	}

	Texture3D::Texture3D(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myTexture(aDevice, anUploader, aMetadata)
	{
		Debug::Assert(
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE3D
			, "Texture3D only support 3D DDS data.");
	}

	Texture3D::Texture3D(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myTexture(aDevice, anUploader, std::forward<std::unique_ptr<DirectX::ScratchImage>>(anImage))
	{
		Debug::Assert(
			myTexture.GetMetadata().dimension == TEX_DIMENSION_TEXTURE3D
			, "Texture3D only support 3D DDS data.");
	}

	void Texture3D::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
	{
		myTexture.Apply(anUpdateMipmaps, aMakeNoLongerReadable);
	}

	TextureDimension Texture3D::GetDimensions() const
	{
		return TextureDimension::Tex2D;
	}

	unsigned int Texture3D::GetDepth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().depth);
	}

	FilterMode Texture3D::GetFilterMode() const
	{
		Debug::LogWarning("Texture3D::GetFilterMode() is not implemented. Defaulting to FilterMode::Point.");
		return FilterMode::Point;
	}

	TextureFormat Texture3D::GetFormat() const
	{
		return ToTextureFormat(myTexture.GetMetadata().format);
	}

	unsigned int Texture3D::GetHeight() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().height);
	}

	bool Texture3D::IsReadable() const
	{
		return myTexture.GetImage() != nullptr;
	}

	unsigned int Texture3D::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().mipLevels);
	}

	unsigned int Texture3D::GetWidth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().width);
	}

	TextureWrapMode Texture3D::GetWrapModeU() const
	{
		Debug::LogWarning("Texture3D::GetWrapModeU() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode Texture3D::GetWrapModeV() const
	{
		Debug::LogWarning("Texture3D::GetWrapModeV() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode Texture3D::GetWrapModeW() const
	{
		Debug::LogWarning("Texture3D::GetWrapModeW() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	void Texture3D::SetFilterMode(FilterMode aFilterMode)
	{
		aFilterMode;
		Debug::LogError("Texture3D::SetFilterMode() is not implemented. Call ignored.");
	}

	void Texture3D::SetWrapMode(TextureWrapMode aWrapMode)
	{
		aWrapMode;
		Debug::LogError("Texture3D::SetWrapMode() is not implemented. Call ignored.");
	}

	void Texture3D::SetWrapModeU(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture3D::SetWrapModeU() is not implemented. Call ignored.");
	}

	void Texture3D::SetWrapModeV(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture3D::SetWrapModeV() is not implemented. Call ignored.");
	}

	void Texture3D::SetWrapModeW(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Texture3D::SetWrapModeW() is not implemented. Call ignored.");
	}

	void* Texture3D::GetNativeTexturePtr() const
	{
		return myTexture.GetResource()->GetResource().Get();
	}

	Color Texture3D::GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel) const
	{
		anX; aY; aZ; aMipLevel;
		return Color::Predefined::Black;
	}

	Color Texture3D::GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel) const
	{
		aU; aV; aW; aMipLevel;
		return Color::Predefined::Black;
	}

	void Texture3D::SetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, const Color& aColor, unsigned int aMipLevel)
	{
		anX; aY; aZ; aColor; aMipLevel;
	}

	TextureCube::TextureCube(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myTexture(aDevice, anUploader, aMetadata)
	{
		Debug::Assert(
			myTexture.GetMetadata().IsCubemap()
			, "TextureCube only support Cubemap DDS data.");
	}

	TextureCube::TextureCube(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myTexture(aDevice, anUploader, std::forward<std::unique_ptr<DirectX::ScratchImage>>(anImage))
	{
		Debug::Assert(
			myTexture.GetMetadata().IsCubemap()
			, "TextureCube only support Cubemap DDS data.");
	}

	void TextureCube::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
	{
		myTexture.Apply(anUpdateMipmaps, aMakeNoLongerReadable);
	}

	TextureDimension TextureCube::GetDimensions() const
	{
		return TextureDimension::Cube;
	}

	unsigned int TextureCube::GetDepth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().depth);
	}

	FilterMode TextureCube::GetFilterMode() const
	{
		Debug::LogWarning("TextureCube::GetFilterMode() is not implemented. Defaulting to FilterMode::Point.");
		return FilterMode::Point;
	}

	TextureFormat TextureCube::GetFormat() const
	{
		return ToTextureFormat(myTexture.GetMetadata().format);
	}

	unsigned int TextureCube::GetHeight() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().height);
	}

	bool TextureCube::IsReadable() const
	{
		return myTexture.GetImage() != nullptr;
	}

	unsigned int TextureCube::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().mipLevels);
	}

	unsigned int TextureCube::GetWidth() const
	{
		return static_cast<unsigned int>(myTexture.GetMetadata().width);
	}

	TextureWrapMode TextureCube::GetWrapModeU() const
	{
		Debug::LogWarning("TextureCube::GetWrapModeU() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode TextureCube::GetWrapModeV() const
	{
		Debug::LogWarning("TextureCube::GetWrapModeV() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	TextureWrapMode TextureCube::GetWrapModeW() const
	{
		Debug::LogWarning("TextureCube::GetWrapModeW() is not implemented. Defaulting to TextureWrapMode::Repeat.");
		return TextureWrapMode::Repeat;
	}

	void TextureCube::SetFilterMode(FilterMode aFilterMode)
	{
		aFilterMode;
		Debug::LogError("TextureCube::SetFilterMode() is not implemented. Call ignored.");
	}

	void TextureCube::SetWrapMode(TextureWrapMode aWrapMode)
	{
		aWrapMode;
		Debug::LogError("TextureCube::SetWrapMode() is not implemented. Call ignored.");
	}

	void TextureCube::SetWrapModeU(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("TextureCube::SetWrapModeU() is not implemented. Call ignored.");
	}

	void TextureCube::SetWrapModeV(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("TextureCube::SetWrapModeV() is not implemented. Call ignored.");
	}

	void TextureCube::SetWrapModeW(TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("TextureCube::SetWrapModeW() is not implemented. Call ignored.");
	}

	void* TextureCube::GetNativeTexturePtr() const
	{
		return myTexture.GetResource()->GetResource().Get();
	}

	Color TextureCube::GetPixel(TextureCubeFace aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel) const
	{
		aFace; anX; aY; aMipLevel;
		return Color::Predefined::Black;
	}

	void TextureCube::SetPixel(TextureCubeFace aFace, unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel)
	{
		aFace; anX; aY; aColor; aMipLevel;
	}
}
