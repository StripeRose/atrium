#pragma once

#include "Graphics_Texture.hpp"

#include <rose-common/Color.hpp>

#include <memory>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class Texture3D : public Texture
	{
	/*public:
		static Resource<Texture3D> Create(
			unsigned int aWidth,
			unsigned int aHeight,
			unsigned int aDepth,
			TextureFormat aFormat = TextureFormat::RGBA32,
			unsigned int aMipCount = -1,
			bool anIsLinear = false
		);*/

	public:
		/*void Apply(bool anUpdateMipmaps = true, bool aMakeNoLongerReadable = false);

		TextureFormat GetFormat() const;
		Color GetPixel(unsigned int anX, unsigned int aY, unsigned int aZ, unsigned int aMipLevel = 0) const;
		Color GetPixelBilinear(float aU, float aV, float aW, unsigned int aMipLevel = 0) const;
		System::Collections::List<Color> GetPixels(unsigned int aMipLevel = 0) const;

		void SetPixel(unsigned int anX, unsigned int aY, Color aColor, unsigned int aMipLevel = 0);
		void SetPixels(const System::Collections::List<Color>& someColors, unsigned int aMipLevel = 0) const;

		void UpdateNativeTexturePtr(NativePtr aNativeTexturePtr);*/

		// Implementing Texture
	public:
	};
}