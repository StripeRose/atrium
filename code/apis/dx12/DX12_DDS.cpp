// Filter "Resources"

#include "stdafx.hpp"

#include "DX12_DDS.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Texture.hpp"

namespace RoseGold::DirectX12
{
	std::shared_ptr<Core::Graphics::Texture> LoadDDSFromFile(const std::filesystem::path& aPath, Device& aDevice, UploadContext& anUploader)
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

		std::shared_ptr<Core::Graphics::Texture> textureInstance;

		switch (backend->GetDimensions())
		{
		case Core::Graphics::TextureDimension::Tex2D:
		{
			Texture2D* tex2d = new DirectX12::Texture2D(backend);
			tex2d->Apply(true, false);
			textureInstance.reset(tex2d);
			break;
		}
		case Core::Graphics::TextureDimension::Tex3D:
		{
			Texture3D* tex3d = new DirectX12::Texture3D(backend);
			tex3d->Apply(true, false);
			textureInstance.reset(tex3d);
			break;
		}
		case Core::Graphics::TextureDimension::Cube:
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
}
