// Filter "Graphics"

#include "Atrium_Graphics.hpp"

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

	void Graphics::ClearColor(const RenderTarget& aTarget, ColorT<float> aClearColor)
	{
		GetGraphicsHandler().GetCurrentFrameContext().ClearColor(aTarget.myTarget, aClearColor);
	}

	void Graphics::ClearColor(const Window& aTarget, ColorT<float> aClearColor)
	{
		GetGraphicsHandler().GetCurrentFrameContext().ClearColor(aTarget.myRenderTarget, aClearColor);
	}

	void Graphics::ClearDepth(const RenderTarget& aTarget, float aDepth, std::uint8_t aStencil)
	{
		GetGraphicsHandler().GetCurrentFrameContext().ClearDepth(aTarget.myTarget, aDepth, aStencil);
	}

	void Graphics::ClearDepth(const Window& aTarget, float aDepth, std::uint8_t aStencil)
	{
		GetGraphicsHandler().GetCurrentFrameContext().ClearDepth(aTarget.myRenderTarget, aDepth, aStencil);
	}
}
