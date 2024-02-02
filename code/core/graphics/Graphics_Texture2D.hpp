#pragma once

#include "Common_Color.hpp"

#include "Graphics_Texture.hpp"

#include <memory>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class Texture2D : public Texture
	{
	public:
		//static Texture2D BlackTexture;
		//static Texture2D GrayTexture;
		//static Texture2D LinearGrayTexture;
		//static Texture2D NormalTexture;
		//static Texture2D RedTexture;
		//static Texture2D WhiteTexture;

	/*public:
		static Resource<Texture2D> Create(
			unsigned int aWidth,
			unsigned int aHeight,
			TextureFormat aFormat = TextureFormat::RGBA32,
			unsigned int aMipCount = -1,
			bool anIsLinear = false);*/

	public:
		virtual void Apply(bool anUpdateMipmaps = true, bool aMakeNoLongerReadable = false) = 0;

		virtual TextureFormat GetFormat() const = 0;
		//virtual Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const = 0;
		//virtual Color GetPixelBilinear(float aU, float aV, unsigned int aMipLevel = 0) const = 0;
		//System::Collections::List<Color> GetPixels(unsigned int aMipLevel = 0) const;
		//System::Collections::List<Color> GetPixels(unsigned int aX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, unsigned int aMipLevel = 0) const;

		//virtual void SetPixel(unsigned int anX, unsigned int aY, Color aColor, unsigned int aMipLevel = 0) = 0;
		//virtual void SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel = 0) const = 0;
		//virtual void SetPixels(unsigned int aX, unsigned int aY, unsigned int aBlockWidth, unsigned int aBlockHeight, const std::vector<Color>& someColors, unsigned int aMipLevel = 0) const = 0;

		// Implementing Texture
	public:
	};
}