#pragma once

#include "Graphics_Texture.hpp"

#include <memory>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class CubemapTexture : public Texture
	{
	public:
		//static Resource<CubemapTexture> Create();

	public:
		virtual ~CubemapTexture() = default;

		virtual void Apply(bool anUpdateMipmaps = true, bool aMakeNoLongerReadable = false) = 0;

		// Implementing Texture
	public:
	};
}