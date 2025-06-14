// Filter "Graphics"

#pragma once

#include "Atrium_WindowManagement.hpp"

#include "Core_RenderTexture.hpp"

#include <memory>

namespace Atrium
{
	using RenderTextureDescriptor = Core::RenderTextureDescriptor;

	class RenderTarget
	{
		friend class Graphics;
	public:
		RenderTarget() = delete; // Todo: Implement actual constructor.

		const RenderTextureDescriptor GetDescriptor() const;

		TextureDimension GetDimensions() const;
		unsigned int GetDepth() const;
		unsigned int GetHeight() const;
		bool IsReadable() const;
		unsigned int GetMipmapCount() const;
		unsigned int GetWidth() const;

		void* GetNativeColorBufferPtr() const;
		void* GetNativeDepthBufferPtr() const;

	private:
		std::shared_ptr<Core::RenderTexture> myTarget;
	};
}