// Filter "Resources"

#include "stdafx.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"
#include "DX12_Manager.hpp"
#include "DX12_MemoryAlignment.hpp"

namespace RoseGold::DirectX12
{
	using namespace DirectX;

	std::shared_ptr<Texture2D_DDS> Texture2D_DDS::LoadFromFile(const std::filesystem::path& aPath, Device& aDevice, UploadContext& anUploader)
	{
		ZoneScoped;

		std::unique_ptr<ScratchImage> image = std::make_unique<ScratchImage>();

		HRESULT loadResult = LoadFromDDSFile(aPath.c_str(), DDS_FLAGS_NONE, nullptr, *image);

		if (!VerifyAction(loadResult, "Load DDS file from disk."))
			return nullptr;

		if (image->GetMetadata().arraySize != 1)
		{
			Debug::LogError("No support for DDS array textures.");
			return nullptr;
		}

		switch (image->GetMetadata().dimension)
		{
		case TEX_DIMENSION_TEXTURE1D:
		case TEX_DIMENSION_TEXTURE2D:
		{
			std::shared_ptr<Texture2D_DDS> ddsInstance(new Texture2D_DDS(aDevice, anUploader));
			ddsInstance->ApplyImage(std::move(image));
			ddsInstance->GetResource()->SetName(aPath.filename().c_str());
			return ddsInstance;
		}
		case TEX_DIMENSION_TEXTURE3D:
			Debug::LogError("No support for 3-dimensional DDS textures.");
			break;
		}

		return nullptr;
	}

	Core::Graphics::TextureDimension Texture2D_DDS::GetDimensions() const
	{
		return Core::Graphics::TextureDimension::Tex2D;
	}

	unsigned int Texture2D_DDS::GetDepth() const
	{
		return static_cast<unsigned int>(myMetadata.depth);
	}

	Core::Graphics::FilterMode Texture2D_DDS::GetFilterMode() const
	{
		Debug::LogFatal("Not implemented.");
		return Core::Graphics::FilterMode::Point;
	}

	unsigned int Texture2D_DDS::GetHeight() const
	{
		return static_cast<unsigned int>(myMetadata.height);
	}

	bool Texture2D_DDS::IsReadable() const
	{
		return myImage != nullptr;
	}

	unsigned int Texture2D_DDS::GetMipmapCount() const
	{
		return static_cast<unsigned int>(myMetadata.mipLevels);
	}

	unsigned int Texture2D_DDS::GetWidth() const
	{
		return static_cast<unsigned int>(myMetadata.width);
	}

	Core::Graphics::TextureWrapMode Texture2D_DDS::GetWrapModeU() const
	{
		Debug::LogFatal("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode Texture2D_DDS::GetWrapModeV() const
	{
		Debug::LogFatal("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode Texture2D_DDS::GetWrapModeW() const
	{
		Debug::LogFatal("Not implemented.");
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	void Texture2D_DDS::SetFilterMode(Core::Graphics::FilterMode aFilterMode)
	{
		aFilterMode;
		Debug::LogFatal("Not implemented.");
	}

	void Texture2D_DDS::SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode)
	{
		aWrapMode;
		Debug::LogFatal("Not implemented.");
	}

	void Texture2D_DDS::SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogFatal("Not implemented.");
	}

	void Texture2D_DDS::SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogFatal("Not implemented.");
	}

	void Texture2D_DDS::SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
		Debug::LogFatal("Not implemented.");
	}

	void* Texture2D_DDS::GetNativeTexturePtr() const
	{
		return myResource.Get();
	}

	// Todo: Keep resource if it's still the right setup.
	void Texture2D_DDS::Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable)
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

	void Texture2D_DDS::Apply_SetupResource()
	{
		D3D12_RESOURCE_DESC textureDesc;
		textureDesc.Format = myMetadata.format;
		textureDesc.Width = static_cast<UINT64>(myMetadata.width);
		textureDesc.Height = static_cast<UINT>(myMetadata.height);
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = static_cast<UINT16>(myMetadata.mipLevels);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		if (myMetadata.dimension == TEX_DIMENSION_TEXTURE2D)
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		else
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;

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

		/*D3D12_SHADER_RESOURCE_VIEW_DESC* srvDescPointer = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		bool isCubeMap = desc.mResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && desc.mResourceDesc.DepthOrArraySize == 6;

		if (isCubeMap)
		{
			shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = desc.mResourceDesc.MipLevels;
			shaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			srvDescPointer = &shaderResourceViewDesc;
		}

		mDevice->CreateShaderResourceView(newTexture->mResource, srvDescPointer, newTexture->mSRVDescriptor.mCPUHandle);*/

		myDevice.GetDevice()->CreateShaderResourceView(
			myResource.Get(),
			nullptr,
			mySRVHandle->GetCPUHandle()
		);
	}

	void Texture2D_DDS::Apply_BeginImageUpload()
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

	Core::Graphics::TextureFormat Texture2D_DDS::GetFormat() const
	{
		return ToTextureFormat(myMetadata.format);
	}

	Texture2D_DDS::Texture2D_DDS(Device& aDevice, UploadContext& anUploader)
		: myDevice(aDevice)
		, myUploader(anUploader)
	{ }

	void Texture2D_DDS::ApplyImage(std::unique_ptr<DirectX::ScratchImage>&& anImage)
	{
		std::swap(myImage, anImage);
		myMetadata = myImage->GetMetadata();
		Apply(false, false);
	}
}
