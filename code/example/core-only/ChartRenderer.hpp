// Filter "Chart/Rendering"
#pragma once

#include "Mesh.hpp"

#include "Common_Math.hpp"

#include "Graphics_FrameContext.hpp"

class ChartController;
class ChartPlayer;
class ChartRenderer
{
public:
	ChartRenderer(ChartPlayer& aPlayer);

	void SetupResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, RoseGold::Core::GraphicsFormat aColorTargetFormat, RoseGold::Core::GraphicsFormat aDepthStencilFormat);

	void Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget);

private:
	void SetupFretboardResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, const std::shared_ptr<RoseGold::Core::RootSignature>& aRootSignature, RoseGold::Core::GraphicsFormat aColorTargetFormat, RoseGold::Core::GraphicsFormat aDepthStencilFormat);
	void RenderController(RoseGold::Core::FrameContext& aContext, ChartController& aController, const RoseGold::Math::Rectangle& aDrawingRect);

	ChartPlayer& myPlayer;

	std::unique_ptr<Mesh> myFretboardMesh;
	std::shared_ptr<RoseGold::Core::PipelineState> myFretboardPipelineState;
	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myFretboardModelViewProjection;
};
