// Filter "Resources"

#pragma once

#include <Graphics_Texture.hpp>

#include <filesystem>

namespace Atrium::DirectX12
{
	class Device;
	class UploadContext;
	std::shared_ptr<Core::Texture> LoadDDSFromFile(Device& aDevice, UploadContext& anUploader, const std::filesystem::path& aPath);

	std::shared_ptr<Core::EditableTexture> CreateEditableDDS(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::TextureFormat aTextureFormat);
}