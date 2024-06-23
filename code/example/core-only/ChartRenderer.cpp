// Filter "Chart/Rendering"
#include "stdafx.hpp"
#include "ChartRenderer.hpp"

ChartRenderer::ChartRenderer(ChartPlayer& aPlayer)
	: myPlayer(aPlayer)
{ }

void ChartRenderer::SetupResources()
{
	ZoneScoped;
}

void ChartRenderer::Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget)
{
	ZoneScoped;

	{
		aContext.SetViewportAndScissorRect(RoseGold::Size(aTarget->GetWidth(), aTarget->GetHeight()));
		aContext.SetRenderTargets({ aTarget }, aTarget);
	}

	// Todo: Render each of the players registered with the ChartPlayer.
}
