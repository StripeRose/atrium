// Filter "Graphics"

#include "Atrium_RenderTarget.hpp"

#include "Core_GraphicsAPI.hpp"

namespace Atrium
{
	extern Core::GraphicsAPI* ourGraphicsHandler;

	namespace
	{
		Core::GraphicsAPI& GetGraphicsHandler()
		{
			return *ourGraphicsHandler;
		}
	}

	const RenderTextureDescriptor RenderTarget::GetDescriptor() const
	{
		return myTarget->GetDescriptor();
	}

	TextureDimension RenderTarget::GetDimensions() const
	{
		return myTarget->GetDimensions();
	}

	unsigned int RenderTarget::GetDepth() const
	{
		return myTarget->GetDepth();
	}

	unsigned int RenderTarget::GetHeight() const
	{
		return myTarget->GetHeight();
	}

	bool RenderTarget::IsReadable() const
	{
		return myTarget->IsReadable();
	}

	unsigned int RenderTarget::GetMipmapCount() const
	{
		return myTarget->GetMipmapCount();
	}

	unsigned int RenderTarget::GetWidth() const
	{
		return myTarget->GetWidth();
	}

	void* RenderTarget::GetNativeColorBufferPtr() const
	{
		return myTarget->GetNativeTexturePtr();
	}

	void* RenderTarget::GetNativeDepthBufferPtr() const
	{
		return myTarget->GetNativeDepthBufferPtr();
	}
}