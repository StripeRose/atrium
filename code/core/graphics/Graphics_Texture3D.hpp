#pragma once

#include "Common_Color.hpp"

#include "Graphics_Texture.hpp"

#include <memory>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class Texture3D : public Texture
	{
	public:
		void Apply() { Apply(true, false); }
		virtual void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel = 0) const = 0;
		virtual Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel = 0) const = 0;
		virtual std::vector<Color> GetPixels(unsigned int aMipLevel = 0) const = 0;

		virtual void SetPixel(unsigned int anX, unsigned int aY, const Color& aColor, unsigned int aMipLevel = 0) = 0;
		virtual void SetPixels(const std::vector<Color>& someColors, unsigned int aMipLevel = 0) const = 0;
	};
}