// Filter "Resources"

#pragma once

#include <Atrium_Texture.hpp>

#include <filesystem>

namespace Atrium::DirectX12
{
	class Device;
	class UploadContext;
	std::shared_ptr<Atrium::Texture> LoadDDSFromFile(Device& aDevice, UploadContext& anUploader, const std::filesystem::path& aPath);

	std::shared_ptr<Atrium::EditableTexture> CreateEditableDDS(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, TextureFormat aTextureFormat);
}