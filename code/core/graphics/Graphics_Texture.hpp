#pragma once

#include "Graphics_Enums.hpp"

namespace RoseGold::Core::Graphics
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual TextureDimension GetDimensions() const = 0;
		virtual unsigned int GetDepth() const = 0;
		virtual FilterMode GetFilterMode() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual bool IsReadable() const = 0;
		virtual unsigned int GetMipmapCount() const = 0;
		virtual unsigned int GetWidth() const = 0;

		virtual TextureWrapMode GetWrapModeU() const = 0;
		virtual TextureWrapMode GetWrapModeV() const = 0;
		virtual TextureWrapMode GetWrapModeW() const = 0;

		virtual void SetFilterMode(FilterMode aFilterMode) = 0;
		virtual void SetWrapMode(TextureWrapMode aWrapMode) = 0;
		virtual void SetWrapModeU(TextureWrapMode aWrapMode) const = 0;
		virtual void SetWrapModeV(TextureWrapMode aWrapMode) const = 0;
		virtual void SetWrapModeW(TextureWrapMode aWrapMode) const = 0;

		/// <summary>
		/// Gives a pointer to the native texture resource of the underlying graphics API.
		/// Ex.
		/// D3D11:  ID3D11Resource*
		/// D3D12:  ID3D12Resource*
		/// Vulkan: VkImage*
		/// </summary>
		virtual void* GetNativeTexturePtr() const = 0;

		/*
		* Todo:
		* Anisotropy level
		*/
	};
}