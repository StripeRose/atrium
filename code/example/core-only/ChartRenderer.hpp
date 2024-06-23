// Filter "Chart/Rendering"
#pragma once

#include "Graphics_FrameContext.hpp"

class ChartPlayer;
class ChartRenderer
{
public:
	ChartRenderer(ChartPlayer& aPlayer);

	void SetupResources();

	void Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget);

private:
	ChartPlayer& myPlayer;
};
