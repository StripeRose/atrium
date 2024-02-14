// Filter "Resources"
#include "stdafx.hpp"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"
#include "DX12_Manager.hpp"
#include "DX12_MemoryAlignment.hpp"
#include "DX12_Texture.hpp"

namespace RoseGold::DirectX12
{
	using namespace DirectX;

	TextureBackend* TextureBackend::FromTexture(Core::Graphics::Texture& aTexture)
	{
		switch (aTexture.GetDimensions())
		{
		case Core::Graphics::TextureDimension::Tex2D:
			return &static_cast<Texture2D&>(aTexture).GetBackend();
		case Core::Graphics::TextureDimension::Tex3D:
			return &static_cast<Texture3D&>(aTexture).GetBackend();
		case Core::Graphics::TextureDimension::Cube:
			return &static_cast<TextureCube&>(aTexture).GetBackend();
		default:
			Debug::LogFatal("Tried to get the texture backend from an unsupported texture.");
			return nullptr;
		}
	}

	const TextureBackend* TextureBackend::FromTexture(const Core::Graphics::Texture& aTexture)
	{
		switch (aTexture.GetDimensions())
		{
		case Core::Graphics::TextureDimension::Tex2D:
			return &static_cast<const Texture2D&>(aTexture).GetBackend();
		case Core::Graphics::TextureDimension::Tex3D:
			return &static_cast<const Texture3D&>(aTexture).GetBackend();
		case Core::Graphics::TextureDimension::Cube:
			return &static_cast<const TextureCube&>(aTexture).GetBackend();
		default:
			Debug::LogFatal("Tried to get the texture backend from an unsupported texture.");
			return nullptr;
		}
	}

	TextureBackend::TextureBackend(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myDevice(aDevice)
		, myUploader(anUploader)
		, myImage(std::make_unique<DirectX::ScratchImage>())
		, myMetadata(aMetadata)
	{
		myImage->Initialize(aMetadata);
	}

	TextureBackend::TextureBackend(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myDevice(aDevice)
		, myUploader(anUploader)
		, myMetadata(anImage->GetMetadata())
	{
		std::swap(myImage, anImage);
	}

	void TextureBackend::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
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

	Core::Graphics::TextureDimension TextureBackend::GetDimensions() const
	{
		switch (myMetadata.dimension)
		{
		case TEX_DIMENSION_TEXTURE1D:
			return Core::Graphics::TextureDimension::Tex2D;
		case TEX_DIMENSION_TEXTURE2D:
			return myMetadata.IsCubemap()
				? Core::Graphics::TextureDimension::Cube
				: Core::Graphics::TextureDimension::Tex2D;
		case TEX_DIMENSION_TEXTURE3D:
			return Core::Graphics::TextureDimension::Tex3D;
		default:
			return Core::Graphics::TextureDimension::Unknown;
		}
	}

	unsigned int TextureBackend::GetDepth() const
	{
		return static_cast<unsigned int>(myMetadata.depth);
	}

	Core::Graphics::FilterMode TextureBackend::GetFilterMode() const
	{
		Debug::LogWarning("Not implemented.");
		return Core::Graphics::FilterMode::Point;
	}

	Core::Graphics::TextureFormat TextureBackend::GetFormat() const
	{
		return ToTextureFormat(myMetadata.format);
	}

	unsigned int TextureBackend::GetHeight() const
	{
		return static_cast<unsigned int>(myMetadata.height);
	}

	bool TextureBackend::IsReadable() const
	{
		return myImage != nullptr;
	}

	unsigned int TextureBackend::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myMetadata.mipLevels);
	}

	unsigned int TextureBackend::GetWidth() const
	{
		return static_cast<unsigned int>(myMetadata.width);
	}

	Core::Graphics::TextureWrapMode TextureBackend::GetWrapModeU() const
	{
		Debug::LogWarning("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode TextureBackend::GetWrapModeV() const
	{
		Debug::LogWarning("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode TextureBackend::GetWrapModeW() const
	{
		Debug::LogWarning("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	void TextureBackend::SetFilterMode(Core::Graphics::FilterMode aFilterMode)
	{
		aFilterMode;
		Debug::LogError("Not implemented.");
	}

	void TextureBackend::SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode)
	{
		aWrapMode;
		Debug::LogError("Not implemented.");
	}

	void TextureBackend::SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Not implemented.");
	}

	void TextureBackend::SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Not implemented.");
	}

	void TextureBackend::SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogError("Not implemented.");
	}

	void* TextureBackend::GetNativeTexturePtr() const
	{
		return myResource.Get();
	}

	// Todo: Keep resource if it's still the right setup.
	void TextureBackend::Apply_SetupResource()
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

		myUsageState = D3D12_RESOURCE_STATE_COPY_DEST;
		myDevice.GetDevice()->CreateCommittedResource(&defaultProperties, D3D12_HEAP_FLAG_NONE, &textureDesc,
			myUsageState, nullptr, IID_PPV_ARGS(myResource.ReleaseAndGetAddressOf()));

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
			myResource.Get(),
			srvDescPtr,
			mySRVHandle->GetCPUHandle()
		);
	}

	void TextureBackend::Apply_BeginImageUpload()
	{
		UploadContext::TextureUpload& textureUpload = myUploader.AddTextureUpload();
		textureUpload.Resource = shared_from_this();
		textureUpload.SubresourceCount = static_cast<std::uint32_t>(myMetadata.mipLevels * myMetadata.arraySize);

		UINT numRows[UploadContext::MaxTextureSubresourceCount];
		std::uint64_t rowSizeInBytes[UploadContext::MaxTextureSubresourceCount];

		D3D12_RESOURCE_DESC resourceDesc = myResource->GetDesc();
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

	Color Texture2D::GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel) const
	{
		//DirectX::ScratchImage& scratchImage = myBackend->GetScratchImage();
		//scratchImage.GetMetadata().

		//myBackend->GetScratchImage()->GetPixels();

		//myBackend->GetScratchImage()->GetImage().
		//myBackend->GetScratchImage()->GetMetadata();

		anX; aY; aMipLevel;
		return Color::Predefined::Black;
	}

	Color Texture2D::GetPixelBilinear(float aU, float aV, unsigned int aMipLevel) const
	{
		aU; aV; aMipLevel;
		return Color::Predefined::Black;
	}

	std::vector<Color> Texture2D::GetPixels(unsigned int aMipLevel) const
	{
		aMipLevel;
		return std::vector<Color>();
	}

	std::vector<Color> Texture2D::GetPixels(unsigned int anX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, unsigned int aMipLevel) const
	{
		anX; aY; aBlockWidth; aBlockHeight; aMipLevel;
		return std::vector<Color>();
	}

	void Texture2D::SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel)
	{
		anX; aY; aColor; aMipLevel;
	}

	void Texture2D::SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel) const
	{
		someColors; aMipLevel;
	}

	void Texture2D::SetPixels(unsigned int anX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, const std::vector<Color>& someColors, unsigned int aMipLevel) const
	{
		anX; aY; aBlockWidth; aBlockHeight; someColors; aMipLevel;
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

	std::vector<Color> Texture3D::GetPixels(unsigned int aMipLevel) const
	{
		aMipLevel;
		return std::vector<Color>();
	}

	void Texture3D::SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel)
	{
		anX; aY; aColor; aMipLevel;
	}

	void Texture3D::SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel) const
	{
		someColors; aMipLevel;
	}

	Color TextureCube::GetPixel(Face aFace, unsigned int anX, unsigned int aY) const
	{
		aFace; anX; aY;
		return Color::Predefined::Black;
	}

	Color TextureCube::GetPixels(Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel) const
	{
		aFace; anX; aY; aMipLevel;
		return Color::Predefined::Black;
	}

	void TextureCube::SetPixel(Face aFace, unsigned int anX, unsigned int aY, const Color& aColor)
	{
		aFace; anX; aY; aColor;
	}

	void TextureCube::SetPixels(const std::vector<Color>& someColors, Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel)
	{
		someColors; aFace; anX; aY; aMipLevel;
	}
}
