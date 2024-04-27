#pragma once

#include "Common_Color.hpp"

#include "Graphics_Texture.hpp"

#include <memory>
#include <vector>

namespace RoseGold::Core
{
	class TextureCube : public Texture
	{
	public:
		enum class Face
		{
			PositiveX, NegativeX,
			PositiveY, NegativeY,
			PositiveZ, NegativeZ
		};

	public:
		virtual ~TextureCube() = default;

		void Apply() { Apply(true, false); }
		virtual void Apply(bool anUpdateMipmaps, bool aMakeNoLongerReadable) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual Color GetPixel(Face aFace, unsigned int anX, unsigned int aY) const = 0;
		virtual Color GetPixels(Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) const = 0;

		virtual void SetPixel(Face aFace, unsigned int anX, unsigned int aY, const Color& aColor) = 0;
		virtual void SetPixels(const std::vector<Color>& someColors, Face aFace, unsigned int anX, unsigned int aY, unsigned int aMipLevel = 0) = 0;
	};
}