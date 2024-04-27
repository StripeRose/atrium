// Filter "Resources"

#include "stdafx.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Texture.hpp"

namespace RoseGold::DirectX12
{
	std::shared_ptr<Core::Texture> LoadDDSFromFile(Device& aDevice, UploadContext& anUploader, const std::filesystem::path& aPath)
	{
		ZoneScoped;

		std::unique_ptr<DirectX::ScratchImage> image = std::make_unique<DirectX::ScratchImage>();

		HRESULT loadResult = DirectX::LoadFromDDSFile(aPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *image);

		if (!VerifyAction(loadResult, "Load DDS file from disk."))
			return nullptr;

		if (!image->GetMetadata().IsCubemap() && image->GetMetadata().arraySize != 1)
		{
			Debug::LogError("No support for DDS array textures.");
			return nullptr;
		}

		std::shared_ptr<TextureBackend> backend(new TextureBackend(aDevice, anUploader, std::move(image)));

		std::shared_ptr<Core::Texture> textureInstance;

		switch (backend->GetDimensions())
		{
		case Core::TextureDimension::Tex2D:
		{
			Texture2D* tex2d = new DirectX12::Texture2D(backend);
			tex2d->Apply(true, false);
			textureInstance.reset(tex2d);
			break;
		}
		case Core::TextureDimension::Tex3D:
		{
			Texture3D* tex3d = new DirectX12::Texture3D(backend);
			tex3d->Apply(true, false);
			textureInstance.reset(tex3d);
			break;
		}
		case Core::TextureDimension::Cube:
		{
			TextureCube* texCube = new DirectX12::TextureCube(backend);
			texCube->Apply(true, false);
			textureInstance.reset(texCube);
			break;
		}
		default:
			return nullptr;
		}

		backend->GetResource()->SetName(aPath.filename().c_str());
		return textureInstance;
	}

	std::shared_ptr<Core::Texture2D> CreateDDS2D(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, Core::TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = 1;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		std::shared_ptr<TextureBackend> backend(new TextureBackend(aDevice, anUploader, metadata));
		return std::make_shared<Texture2D>(backend);
	}

	std::shared_ptr<Core::Texture3D> CreateDDS3D(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = aDepth;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE3D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		std::shared_ptr<TextureBackend> backend(new TextureBackend(aDevice, anUploader, metadata));
		return std::make_shared<Texture3D>(backend);
	}

	std::shared_ptr<Core::TextureCube> CreateDDSCube(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, Core::TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 6;
		metadata.depth = 1;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aWidth;
		metadata.mipLevels = 0;
		metadata.miscFlags = DirectX::TEX_MISC_TEXTURECUBE;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		std::shared_ptr<TextureBackend> backend(new TextureBackend(aDevice, anUploader, metadata));
		return std::make_shared<TextureCube>(backend);
	}
}
