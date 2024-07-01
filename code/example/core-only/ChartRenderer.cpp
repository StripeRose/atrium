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

void ChartRenderer::SetupResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, RoseGold::Core::GraphicsFormat aColorTargetFormat)
{
	ZoneScoped;

	std::unique_ptr<RoseGold::Core::RootSignatureBuilder> builder = aGraphicsAPI.GetResourceManager().CreateRootSignature();

	builder->SetVisibility(RoseGold::Core::ShaderVisibility::Vertex);

	// Model, view, projection.
	builder->AddTable().AddCBVRange(1, 0, RoseGold::Core::ResourceUpdateFrequency::PerObject);
	builder->AddTable().AddCBVRange(1, 0, RoseGold::Core::ResourceUpdateFrequency::PerFrame);

	builder->SetVisibility(RoseGold::Core::ShaderVisibility::Pixel);

	builder->AddTable().AddSRVRange(4, 0, RoseGold::Core::ResourceUpdateFrequency::PerMaterial);

	builder->AddSampler(0) // Clamping point
		.Filter(RoseGold::Core::FilterMode::Point)
		.Address(RoseGold::Core::TextureWrapMode::Clamp)
		;

	builder->AddSampler(1) // Clamping linear
		.Filter(RoseGold::Core::FilterMode::Bilinear)
		.Address(RoseGold::Core::TextureWrapMode::Clamp)
		;

	std::shared_ptr<RoseGold::Core::RootSignature> rootSignature = builder->Finalize();

	SetupQuadResources(aGraphicsAPI, rootSignature, aColorTargetFormat);
	SetupFretboardResources(aGraphicsAPI, rootSignature, aColorTargetFormat);
}

void ChartRenderer::Render(RoseGold::Core::FrameContext& aContext, const std::shared_ptr<RoseGold::Core::RenderTexture>& aTarget)
{
	ZoneScoped;

	myLastQuadFlush = 0;
	myQuadInstanceData.clear();

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

		aContext.SetViewport(RoseGold::Math::Rectangle::FromExtents(
			{ column * Size, row * Size },
			{ (column + 1) * Size, (row + 1) * Size }
		));

		RenderController(aContext, *controller);
		FlushQuads(aContext);
	}

	myQuadInstanceBuffer->SetData<QuadInstanceData>(myQuadInstanceData);
}

void ChartRenderer::SetupQuadResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, const std::shared_ptr<RoseGold::Core::RootSignature>& aRootSignature, RoseGold::Core::GraphicsFormat aColorTargetFormat)
{
	ZoneScoped;

	myQuadMesh = CreateQuadMesh(aGraphicsAPI);
	myQuadMesh->SetName(L"Quad");

	RoseGold::Core::PipelineStateDescription pipelineDescription;
	pipelineDescription.RootSignature = aRootSignature;
	pipelineDescription.InputLayout = ChartQuadVertex::GetInputLayout();
	const std::filesystem::path shaderPath = "example/ChartQuad.hlsl";
	pipelineDescription.VertexShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Vertex, "vertexShader");
	pipelineDescription.PixelShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Pixel, "pixelShader");
	pipelineDescription.OutputFormats = { aColorTargetFormat };
	myQuadPipelineState = aGraphicsAPI.GetResourceManager().CreatePipelineState(pipelineDescription);

	struct CameraMatrices
	{
		RoseGold::Math::Matrix View;
		RoseGold::Math::Matrix Projection;
	} cameraMatrices;

	locMakeCameraMatrices(cameraMatrices.View, cameraMatrices.Projection);

	myCameraMatrices = aGraphicsAPI.GetResourceManager().CreateGraphicsBuffer(RoseGold::Core::GraphicsBuffer::Target::Constant, 1, sizeof(CameraMatrices));
	myCameraMatrices->SetData(&cameraMatrices, sizeof(CameraMatrices));

	myQuadInstanceBuffer = aGraphicsAPI.GetResourceManager().CreateGraphicsBuffer(RoseGold::Core::GraphicsBuffer::Target::Vertex, 64, sizeof(QuadInstanceData));
}

void ChartRenderer::SetupFretboardResources(RoseGold::Core::GraphicsAPI& aGraphicsAPI, const std::shared_ptr<RoseGold::Core::RootSignature>& aRootSignature, RoseGold::Core::GraphicsFormat aColorTargetFormat)
{
	ZoneScoped;

	myFretboardMesh = CreateFretboardMesh(aGraphicsAPI);
	myFretboardMesh->SetName(L"Fretboard vertices");

	RoseGold::Core::PipelineStateDescription fretboardPipelineDescription;
	fretboardPipelineDescription.RootSignature = aRootSignature;
	fretboardPipelineDescription.InputLayout = ChartFretboardVertex::GetInputLayout();
	const std::filesystem::path shaderPath = "example/ChartFretboard.hlsl";
	fretboardPipelineDescription.VertexShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Vertex, "vertexShader");
	fretboardPipelineDescription.PixelShader = aGraphicsAPI.GetResourceManager().CreateShader(shaderPath, RoseGold::Core::Shader::Type::Pixel, "pixelShader");
	fretboardPipelineDescription.OutputFormats = { aColorTargetFormat };
	myFretboardPipelineState = aGraphicsAPI.GetResourceManager().CreatePipelineState(fretboardPipelineDescription);

	ModelViewProjection fretboardMatrices;
	fretboardMatrices.Model = RoseGold::Math::Matrix::Identity();
	locMakeCameraMatrices(fretboardMatrices.View, fretboardMatrices.Projection);

	myFretboardModelViewProjection = aGraphicsAPI.GetResourceManager().CreateGraphicsBuffer(RoseGold::Core::GraphicsBuffer::Target::Constant, 1, sizeof(ModelViewProjection));
	myFretboardModelViewProjection->SetData(&fretboardMatrices, sizeof(ModelViewProjection));
}

void ChartRenderer::RenderController(RoseGold::Core::FrameContext& aContext, ChartController& /*aController*/)
{
	ZoneScoped;

	aContext.SetPipelineState(myFretboardPipelineState);
	aContext.SetPipelineResource(RoseGold::Core::ResourceUpdateFrequency::PerObject, 0, myFretboardModelViewProjection);
	myFretboardMesh->DrawToFrame(aContext);

	QueueQuad(RoseGold::Math::MakeMatrix::Translation(-2, 0, 2), RoseGold::Color32(0xFF00FF00), RoseGold::Math::Rectangle::FromExtents({ 0.f, 0.f }, { 1.f, 1.f }));
	QueueQuad(RoseGold::Math::MakeMatrix::Translation(-1, 0, 3), RoseGold::Color32(0xFFFF0000), RoseGold::Math::Rectangle::FromExtents({ 0.f, 0.f }, { 0.5f, 1.f }));
	QueueQuad(RoseGold::Math::MakeMatrix::Translation(0, 0, 4), RoseGold::Color32(0xFFFFFF00), RoseGold::Math::Rectangle::FromExtents({ 0.5f, 0.f }, { 1.f, 1.f }));
	QueueQuad(RoseGold::Math::MakeMatrix::Translation(1, 0, 5), RoseGold::Color32(0xFF008AFF), RoseGold::Math::Rectangle::FromExtents({ 0.f, 0.f }, { 0.5f, 1.f }));
	QueueQuad(RoseGold::Math::MakeMatrix::Translation(2, 0, 6), RoseGold::Color32(0xFFFFB300), RoseGold::Math::Rectangle::FromExtents({ 0.f, 0.f }, { 0.5f, 1.f }));
}

void ChartRenderer::QueueQuad(RoseGold::Math::Matrix aTransform, std::optional<RoseGold::Color32> aColor, std::optional<RoseGold::Math::Rectangle> aUVRectangle)
{
	QuadInstanceData& instance = myQuadInstanceData.emplace_back();
	instance.Transform = aTransform;

	const RoseGold::Color32 color = aColor.value_or(RoseGold::Color32::Predefined::White);
	instance.Color[0] = static_cast<float>(color.R) / 255.f;
	instance.Color[1] = static_cast<float>(color.G) / 255.f;
	instance.Color[2] = static_cast<float>(color.B) / 255.f;
	instance.Color[3] = static_cast<float>(color.A) / 255.f;

	instance.UVMin = { 0, 0 };
	instance.UVMax = { 1, 1 };
	if (aUVRectangle.has_value())
		aUVRectangle.value().ToExtents(instance.UVMin, instance.UVMax);
}

void ChartRenderer::FlushQuads(RoseGold::Core::FrameContext& aContext)
{
	const std::size_t queuedSinceLastFlush = myQuadInstanceData.size() - myLastQuadFlush;
	if (queuedSinceLastFlush == 0)
		return;

	aContext.SetPipelineState(myQuadPipelineState);
	aContext.SetPipelineResource(RoseGold::Core::ResourceUpdateFrequency::PerFrame, 0, myCameraMatrices);

	aContext.SetVertexBuffer(myQuadInstanceBuffer, 1);
	myQuadMesh->DrawInstancedToFrame(aContext, static_cast<unsigned int>(queuedSinceLastFlush), static_cast<unsigned int>(myLastQuadFlush));

	myLastQuadFlush = myQuadInstanceData.size();
}
