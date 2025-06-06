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
		virtual unsigned int GetHeight() const = 0;
		virtual bool IsReadable() const = 0;
		virtual unsigned int GetMipmapCount() const = 0;
		virtual unsigned int GetWidth() const = 0;

		/// <summary>
		/// Gives a pointer to the native texture resource of the underlying graphics API.
		/// Ex.
		/// D3D11:  ID3D11Resource*
		/// D3D12:  ID3D12Resource*
		/// Vulkan: VkImage*
		/// </summary>
		virtual void* GetNativeTexturePtr() const = 0;
	};
}