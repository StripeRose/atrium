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

	Texture::Texture(Device& aDevice, UploadContext& anUploader, const DirectX::TexMetadata& aMetadata)
		: myImage(aDevice, anUploader, aMetadata)
	{

	}

	Texture::Texture(Device& aDevice, UploadContext& anUploader, std::unique_ptr<DirectX::ScratchImage>&& anImage)
		: myImage(aDevice, anUploader, std::forward<std::unique_ptr<DirectX::ScratchImage>>(anImage))
	{

	}

	void Texture::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
	{
		myImage.Apply(anUpdateMipmaps, aMakeNoLongerReadable);
	}

	TextureDimension Texture::GetDimensions() const
	{
		switch (myImage.GetMetadata().dimension)
		{
			case TEX_DIMENSION_TEXTURE1D:
			case TEX_DIMENSION_TEXTURE2D:
				if ((myImage.GetMetadata().miscFlags & DirectX::TEX_MISC_TEXTURECUBE) != 0)
					return myImage.GetMetadata().arraySize ? TextureDimension::CubeArray : TextureDimension::Cube;
				else
					return TextureDimension::Tex2D;
			case TEX_DIMENSION_TEXTURE3D:
				return TextureDimension::Tex3D;
			default:
				return TextureDimension::Unknown;
		}
	}

	unsigned int Texture::GetDepth() const
	{
		return static_cast<unsigned int>(myImage.GetMetadata().depth);
	}

	unsigned int Texture::GetHeight() const
	{
		return static_cast<unsigned int>(myImage.GetMetadata().height);
	}

	bool Texture::IsReadable() const
	{
		return myImage.GetImage() != nullptr;
	}

	unsigned int Texture::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myImage.GetMetadata().mipLevels);
	}

	unsigned int Texture::GetWidth() const
	{
		return static_cast<unsigned int>(myImage.GetMetadata().width);
	}

	void* Texture::GetNativeTexturePtr() const
	{
		return myImage.GetResource()->GetResource().Get();
	}
}
