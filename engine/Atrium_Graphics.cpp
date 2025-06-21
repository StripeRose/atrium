// Filter "Graphics"

#include "Atrium_Graphics.hpp"

#include "Core_GraphicsAPI.hpp"

namespace Atrium
{
	extern Core::FrameGraphicsContext* ourFrameGraphicsContext;

	void Graphics::ClearColor(const RenderTarget& aTarget, ColorT<float> aClearColor)
	{
		ourFrameGraphicsContext->ClearColor(aTarget.myTarget, aClearColor);
	}

	void Graphics::ClearColor(const Window& aTarget, ColorT<float> aClearColor)
	{
		ourFrameGraphicsContext->ClearColor(aTarget.myRenderTarget, aClearColor);
	}

	void Graphics::ClearDepth(const RenderTarget& aTarget, float aDepth, std::uint8_t aStencil)
	{
		ourFrameGraphicsContext->ClearDepth(aTarget.myTarget, aDepth, aStencil);
	}

	void Graphics::ClearDepth(const Window& aTarget, float aDepth, std::uint8_t aStencil)
	{
		ourFrameGraphicsContext->ClearDepth(aTarget.myRenderTarget, aDepth, aStencil);
	}
}
