// Filter "Resources"

#pragma once

#include <Graphics_Texture.hpp>

#include <filesystem>

namespace RoseGold::DirectX12
{
	class Device;
	class UploadContext;
	std::shared_ptr<Core::Graphics::Texture> LoadDDSFromFile(const std::filesystem::path& aPath, Device& aDevice, UploadContext& anUploader);
}