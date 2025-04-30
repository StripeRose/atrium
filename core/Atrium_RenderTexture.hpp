// Filter "Graphics"

#pragma once

#include "Atrium_Texture.hpp"

#include <memory>

namespace Atrium
{
	struct RenderTextureDescriptor
	{
		RenderTextureFormat ColorFormat = RenderTextureFormat::Default;
		GraphicsFormat ColorGraphicsFormat = GraphicsFormat::None;
		//unsigned int DepthBitDepth = 0;
		GraphicsFormat DepthStencilFormat = GraphicsFormat::None;
		TextureDimension Dimension = TextureDimension::Unknown;
		bool IsSRGB = false;

		unsigned int Size_Depth = 0;
		unsigned int Size_Height = 0;
		unsigned int Size_Width = 0;
	};

	class RenderTexture : public Texture
	{
	public:
		virtual const RenderTextureDescriptor& GetDescriptor() const = 0;

		virtual void* GetNativeDepthBufferPtr() const = 0;
	};
}