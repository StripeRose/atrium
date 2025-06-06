// Filter "Graphics"

#pragma once

#include <rose-common/Color.hpp>

#include "Core_GraphicsEnums.hpp"

namespace Atrium::Core
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

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		void Apply() { Apply(true, false); }
		virtual void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual ColorT<float> GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const = 0;
		virtual ColorT<float> GetPixelBilinear(float aU, float aV, unsigned int aMipLevel = 0) const = 0;

		virtual void SetPixel(unsigned int anX, unsigned int aY, const ColorT<float>& aColor, unsigned int aMipLevel = 0) = 0;
	};

	class Texture3D : public Texture
	{
	public:
		virtual ~Texture3D() = default;

		void Apply() { Apply(true, false); }
		virtual void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual ColorT<float> GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel = 0) const = 0;
		virtual ColorT<float> GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel = 0) const = 0;

		virtual void SetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, const ColorT<float>& aColor, unsigned int aMipLevel = 0) = 0;
	};

	class TextureCube : public Texture
	{
	public:
		virtual ~TextureCube() = default;

		void Apply() { Apply(true, false); }
		virtual void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual ColorT<float> GetPixel(TextureCubeFace aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const = 0;

		virtual void SetPixel(TextureCubeFace aFace, unsigned int anX, unsigned int aY, const ColorT<float>& aColor, unsigned int aMipLevel = 0) = 0;
	};
}