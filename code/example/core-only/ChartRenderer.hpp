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

	void SetupResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, RoseGold::Core::GraphicsFormat aColorTargetFormat);

	void Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget);

private:
	struct QuadInstanceData
	{
		RoseGold::Math::Matrix Transform;
		RoseGold::Math::Vector2 UVMin;
		RoseGold::Math::Vector2 UVMax;
		float Color[4];
	};

private:
	void SetupQuadResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, const std::shared_ptr<RoseGold::Core::RootSignature>& aRootSignature, RoseGold::Core::GraphicsFormat aColorTargetFormat);
	void SetupFretboardResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, const std::shared_ptr<RoseGold::Core::RootSignature>& aRootSignature, RoseGold::Core::GraphicsFormat aColorTargetFormat);
	void RenderController(RoseGold::Core::FrameContext& aContext, ChartController& aController);

	void QueueQuad(RoseGold::Math::Matrix aTransform, std::optional<RoseGold::Color32> aColor, std::optional<RoseGold::Math::Rectangle> aUVRectangle);
	void FlushQuads(RoseGold::Core::FrameContext& aContext);

	ChartPlayer& myPlayer;

	std::unique_ptr<Mesh> myQuadMesh;
	std::shared_ptr<RoseGold::Core::PipelineState> myQuadPipelineState;

	std::vector<QuadInstanceData> myQuadInstanceData;
	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myQuadInstanceBuffer;
	std::size_t myLastQuadFlush = 0;

	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myCameraMatrices;

	std::unique_ptr<Mesh> myFretboardMesh;
	std::shared_ptr<RoseGold::Core::PipelineState> myFretboardPipelineState;
	std::shared_ptr<RoseGold::Core::GraphicsBuffer> myFretboardModelViewProjection;
};
