// Filter "Resources"

#pragma once

#include <Graphics_Texture.hpp>
#include <Graphics_Texture2D.hpp>
#include <Graphics_Texture3D.hpp>
#include <Graphics_TextureCube.hpp>

#include <filesystem>

namespace RoseGold::DirectX12
{
	class Device;
	class UploadContext;
	std::shared_ptr<Core::Graphics::Texture> LoadDDSFromFile(Device& aDevice, UploadContext& anUploader, const std::filesystem::path& aPath);

	std::shared_ptr<Core::Graphics::Texture2D> CreateDDS2D(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, Core::Graphics::TextureFormat aTextureFormat);
	std::shared_ptr<Core::Graphics::Texture3D> CreateDDS3D(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, unsigned int aHeight, unsigned int aDepth, Core::Graphics::TextureFormat aTextureFormat);
	std::shared_ptr<Core::Graphics::TextureCube> CreateDDSCube(Device& aDevice, UploadContext& anUploader, unsigned int aWidth, Core::Graphics::TextureFormat aTextureFormat);
}