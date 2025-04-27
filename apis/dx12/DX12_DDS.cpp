// Filter "Resources"

#include "DX12_DDS.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Texture.hpp"

namespace Atrium::DirectX12
{
	std::shared_ptr<Core::Texture> LoadDDSFromFile(Device& aDevice, UploadContext& anUploader, const std::filesystem::path& aPath)
	{
		ZoneScoped;

		std::unique_ptr<DirectX::ScratchImage> image = std::make_unique<DirectX::ScratchImage>();

		HRESULT loadResult = DirectX::LoadFromDDSFile(aPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *image);

		if (!Debug::Verify(loadResult, "Load DDS file from disk."))
			return nullptr;

		if (!image->GetMetadata().IsCubemap() && image->GetMetadata().arraySize != 1)
		{
			Debug::LogError("No support for DDS array textures.");
			return nullptr;
		}

		std::shared_ptr<SimpleTexture> texture = std::make_shared<SimpleTexture>(aDevice, anUploader, std::move(image));
		texture->Apply(true, false);
		texture->GetResource()->SetName(aPath.filename().c_str());

		return texture;
	}

	std::shared_ptr<Core::EditableTexture> CreateEditableDDS(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat)
	{
		DirectX::TexMetadata metadata;
		metadata.arraySize = 1;
		metadata.depth = aDepth;
		metadata.dimension = (aDepth == 1) ? DirectX::TEX_DIMENSION_TEXTURE2D : DirectX::TEX_DIMENSION_TEXTURE3D;
		metadata.format = ToDXGIFormat(ToGraphicsFormat(aTextureFormat));
		metadata.height = aHeight;
		metadata.mipLevels = 0;
		metadata.miscFlags = (aDepth == 6) ? DirectX::TEX_MISC_TEXTURECUBE : 0;
		metadata.miscFlags2 = 0;
		metadata.width = aWidth;

		return std::make_shared<SimpleTexture>(aDevice, anUploader, metadata);
	}
}
