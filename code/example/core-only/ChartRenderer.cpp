// Filter "Chart/Rendering"
#include "stdafx.hpp"
#include "ChartRenderer.hpp"

#include "ChartMeshes.hpp"
#include "ChartPlayer.hpp"

struct ModelViewProjection
{
	RoseGold::Math::Matrix Model;
	RoseGold::Math::Matrix View;
	RoseGold::Math::Matrix Projection;
};

void locMakeCameraMatrices(RoseGold::Math::Matrix& aView, RoseGold::Math::Matrix& aProjection)
{
	constexpr RoseGold::Math::Matrix viewMatrix = (
		RoseGold::Math::MakeMatrix::RotationX(RoseGold::Math::ToRadians(35.f)) *
		RoseGold::Math::MakeMatrix::Translation(0, 1.3f, 0.f)
		).Invert().value();

	constexpr RoseGold::Math::Matrix projectionMatrix = RoseGold::Math::MakeMatrix::PerspectiveFieldOfView(
		RoseGold::Math::ToRadians(90.f), 1.f, 0.0001f, 100.f
	);

	aView = viewMatrix;
	aProjection = projectionMatrix;
}

ChartRenderer::ChartRenderer(ChartPlayer& aPlayer)
	: myPlayer(aPlayer)
{ }

void ChartRenderer::SetupResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, RoseGold::Core::GraphicsFormat aColorTargetFormat, RoseGold::Core::GraphicsFormat aDepthStencilFormat)
{
	ZoneScoped;

	SetupFretboardResources(aGraphicsAPI, aColorTargetFormat, aDepthStencilFormat);
}

void ChartRenderer::Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget)
{
	ZoneScoped;

	aContext.SetRenderTargets({ aTarget }, aTarget);
	aContext.SetViewportAndScissorRect(RoseGold::Size(aTarget->GetWidth(), aTarget->GetHeight()));

	const float Size = 300.f;

	const int gridColumns = RoseGold::Math::TruncateTo<int>(aTarget->GetWidth() / Size);

	const std::vector<std::unique_ptr<ChartController>>& controllers = myPlayer.GetControllers();
	for (unsigned int i = 0; i < controllers.size(); ++i)
	{
		const int column = i % gridColumns;
		const int row = (i - column) / gridColumns;

		const std::unique_ptr<ChartController>& controller = controllers.at(i);

		RenderController(
			aContext,
			*controller,
			RoseGold::Math::Rectangle::FromExtents(
				{ column * Size, row * Size },
				{ (column + 1) * Size, (row + 1) * Size }
			)
		);
	}
}

void ChartRenderer::SetupFretboardResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, RoseGold::Core::GraphicsFormat aColorTargetFormat, RoseGold::Core::GraphicsFormat aDepthStencilFormat)
{
	ZoneScoped;

	myFretboardMesh = CreateFretboardMesh(aGraphicsAPI);
	myFretboardMesh->SetName(L"Fretboard vertices");

	RoseGold::Core::PipelineStateDescription fretboardPipelineDescription;
	fretboardPipelineDescription.InputLayout = ChartFretboardVertex::GetInputLayout();
	const std::filesystem::path shaderPath = "example/ChartFretboard.hlsl";
	fretboardPipelineDescription.VertexShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Vertex, "vertexShader");
	fretboardPipelineDescription.PixelShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Pixel, "pixelShader");
	fretboardPipelineDescription.OutputFormats = { aColorTargetFormat };
	fretboardPipelineDescription.DepthTargetFormat = aDepthStencilFormat;
	myFretboardPipelineState = aGraphicsAPI.GetResourceManager().CreateOrGetPipelineState(fretboardPipelineDescription);

	ModelViewProjection fretboardMatrices;
	fretboardMatrices.Model = RoseGold::Math::Matrix::Identity();
	locMakeCameraMatrices(fretboardMatrices.View, fretboardMatrices.Projection);

	myFretboardModelViewProjection = aGraphicsAPI.GetResourceManager().CreateGraphicsBuffer(RoseGold::Core::GraphicsBuffer::Target::Constant, 1, sizeof(ModelViewProjection));
	myFretboardModelViewProjection->SetData(&fretboardMatrices, sizeof(ModelViewProjection));
}

void ChartRenderer::RenderController(RoseGold::Core::FrameContext& aContext, ChartController& /*aController*/, const RoseGold::Math::Rectangle& aDrawingRect)
{
	ZoneScoped;

	aContext.SetViewport(aDrawingRect);

	aContext.SetPipelineState(myFretboardPipelineState);
	aContext.SetPipelineResource(RoseGold::Core::ResourceUpdateFrequency::PerObject, 0, myFretboardModelViewProjection);
	myFretboardMesh->DrawToFrame(aContext);
}
