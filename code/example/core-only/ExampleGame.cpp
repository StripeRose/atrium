#include "stdafx.hpp"

#include "ExampleGame.hpp"

#include <Common_Debug.hpp>
#include <Common_Profiling.hpp>

std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> TexturedVertex::GetInputLayout()
{
	std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> layout;
	layout.emplace_back("POSITION", RoseGold::Core::Graphics::GraphicsFormat::R32G32B32_SFloat);
	layout.emplace_back("TEXCOORD", RoseGold::Core::Graphics::GraphicsFormat::R32G32_SFloat);
	return layout;
}

std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> ColoredVertex::GetInputLayout()
{
	std::vector<RoseGold::Core::Graphics::PipelineStateDescription::InputLayoutEntry> layout;
	layout.emplace_back("POSITION", RoseGold::Core::Graphics::GraphicsFormat::R32G32B32_SFloat);
	layout.emplace_back("COLOR", RoseGold::Core::Graphics::GraphicsFormat::R32G32B32A32_SFloat);
	return layout;
}

void ExampleGame::OnStart(RoseGold::Client::BootstrapResult& aCoreSetup)
{
	ZoneScoped;

	// Store the manager-pointers to not have to pass it around constantly.
	myCoreSetup = &aCoreSetup;

	// Load the textures.
	myWhite = aCoreSetup.GraphicsManager->LoadTexture(L"debug/images/10.dds");
	myCheckerboard = aCoreSetup.GraphicsManager->LoadTexture(L"debug/images/checkered4.dds");

	// Set up the windows and create render textures for them.
	OnStart_SetupWindows();

	// Set up the example meshes.
	OnStart_DefineMeshes();

	// Set up pipeline shader combination states.
	OnStart_SetupPipelineStates();

	// Create constant-data buffers to store the model-view-projection data for each mesh in each viewport.
	OnStart_CreateMVPBuffers();

	// Record when we started so our simple animations have a point in time to reference.
	myStartTime = std::chrono::high_resolution_clock::now();
}

void ExampleGame::OnLoop()
{
	ZoneScoped;

	const std::chrono::high_resolution_clock::duration timeSinceStart = (std::chrono::high_resolution_clock::now() - myStartTime);
	const std::chrono::milliseconds msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart);
	const float secondsSinceStart = msSinceStart.count() / 1000.f;

	using namespace RoseGold::Core::Graphics;
	FrameContext& frameContext = myCoreSetup->GraphicsManager->GetCurrentFrameContext();

	if (myWindow1)
	{
		ZoneScopedN("Window 1");
		CONTEXT_ZONE(frameContext, "Window 1");

		// Clear to a basic colour.
		{
			CONTEXT_ZONE(frameContext, "Clear");
			frameContext.ClearColor(myWindow1, RoseGold::Color::Predefined::CornflowerBlue);
			frameContext.ClearDepth(myWindow1, 1.f, 0);
		}

		// Set up the window to be drawn to.
		{
			CONTEXT_ZONE(frameContext, "Set up target");
			frameContext.SetViewportAndScissorRect(RoseGold::Size(myWindow1->GetWidth(), myWindow1->GetHeight()));
			frameContext.SetRenderTargets({ myWindow1 }, myWindow1);
		}

		// Set up some common data.
		CameraConstants mvp;
		{
			CONTEXT_ZONE(frameContext, "Set frame data");
			const float aspectRatio = static_cast<float>(myWindow1->GetWidth()) / static_cast<float>(myWindow1->GetHeight());
			mvp.Projection = RoseGold::Math::MakeMatrix::PerspectiveFieldOfView(RoseGold::Math::ToRadians<float>(60.f), aspectRatio, 0.01f, 50.f);
			mvp.View = RoseGold::Math::MakeMatrix::LookAt({ 2, 2, -3 }, { 0, 0, 0 }, RoseGold::Math::Vector3::Up());
			mvp.View = mvp.View.Invert().value_or(RoseGold::Math::Matrix::Identity());
		}

		// Draw the colored triangle.
		{
			CONTEXT_ZONE(frameContext, "Draw triangle");
			frameContext.SetPipelineState(myColoredMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myWhite);
			mvp.Model =
				RoseGold::Math::MakeMatrix::Translation(0, 0, -0.25f) *
				RoseGold::Math::MakeMatrix::Scale(10) *
				RoseGold::Math::MakeMatrix::Translation(0, -2, 0);
			myRT1TriangleConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT1TriangleConstants);
			myColoredTriangle.DrawToFrame(frameContext);
		}

		// Draw the colored plane.
		{
			CONTEXT_ZONE(frameContext, "Draw plane");
			frameContext.SetPipelineState(myColoredMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myWhite);
			mvp.Model = RoseGold::Math::MakeMatrix::Translation(0, -1.5f, 0);
			myRT1PlaneConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT1PlaneConstants);
			myColoredPlane.DrawToFrame(frameContext);
		}

		// Draw the textured pyramid.
		{
			CONTEXT_ZONE(frameContext, "Draw pyramid");
			frameContext.SetPipelineState(myTexturedMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myCheckerboard);
			mvp.Model = RoseGold::Math::MakeMatrix::RotationY(secondsSinceStart);
			myRT1PyramidConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT1PyramidConstants);
			myTexturedPyramid.DrawToFrame(frameContext);
		}
	}

	if (myWindow2)
	{
		ZoneScopedN("Window 2");
		CONTEXT_ZONE(frameContext, "Window 2");

		// Clear to a basic colour.
		{
			CONTEXT_ZONE(frameContext, "Clear");
			frameContext.ClearColor(myWindow2, RoseGold::Color::Predefined::Tan);
			frameContext.ClearDepth(myWindow2, 1.f, 0);
		}

		// Set up the window to be drawn to.
		{
			CONTEXT_ZONE(frameContext, "Set up target");
			frameContext.SetViewportAndScissorRect(RoseGold::Size(myWindow2->GetWidth(), myWindow2->GetHeight()));
			frameContext.SetRenderTargets({ myWindow2 }, myWindow2);
		}

		// Set up some common data.
		CameraConstants mvp;
		{
			CONTEXT_ZONE(frameContext, "Set frame data");
			const float aspectRatio = static_cast<float>(myWindow2->GetWidth()) / static_cast<float>(myWindow2->GetHeight());
			mvp.Projection = RoseGold::Math::MakeMatrix::PerspectiveFieldOfView(RoseGold::Math::ToRadians<float>(60.f), aspectRatio, 0.01f, 50.f);
			mvp.View = RoseGold::Math::MakeMatrix::LookAt({ 2, 2, -3 }, { 0, 0, 0 }, RoseGold::Math::Vector3::Up());
			mvp.View = mvp.View.Invert().value_or(RoseGold::Math::Matrix::Identity());
		}

		// Draw the colored triangle.
		{
			CONTEXT_ZONE(frameContext, "Draw triangle");
			frameContext.SetPipelineState(myColoredMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myWhite);
			mvp.Model = RoseGold::Math::MakeMatrix::RotationX(RoseGold::Math::ToRadians(-90.f)) * RoseGold::Math::MakeMatrix::Translation(0, 0, 5.f);
			myRT2TriangleConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT2TriangleConstants);
			myColoredTriangle.DrawToFrame(frameContext);
		}

		// Draw the colored plane.
		{
			CONTEXT_ZONE(frameContext, "Draw plane");
			frameContext.SetPipelineState(myColoredMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myWhite);
			mvp.Model = RoseGold::Math::MakeMatrix::RotationY(secondsSinceStart * 0.2f) * RoseGold::Math::MakeMatrix::Scale(20.f);
			myRT2PlaneConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT2PlaneConstants);
			myColoredPlane.DrawToFrame(frameContext);
		}

		// Draw the textured pyramid.
		{
			CONTEXT_ZONE(frameContext, "Draw pyramid");
			frameContext.SetPipelineState(myTexturedMeshPipelineState);
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerMaterial, 0, myCheckerboard);
			mvp.Model =
				RoseGold::Math::MakeMatrix::Translation(0, -0.25f, 0) *
				RoseGold::Math::MakeMatrix::RotationX(secondsSinceStart) *
				RoseGold::Math::MakeMatrix::Translation(0, 1.f, 0);
			myRT2PyramidConstants->SetData(&mvp, sizeof(mvp));
			frameContext.SetPipelineResource(ResourceUpdateFrequency::PerObject, 0, myRT2PyramidConstants);
			myTexturedPyramid.DrawToFrame(frameContext);
		}
	}
}

void ExampleGame::OnExit()
{
	ZoneScoped;
}

void ExampleGame::OnStart_SetupWindows()
{
	ZoneScoped;
	RoseGold::Core::Platform::WindowManager::CreationParameters windowParams;
	windowParams.Title = "Window 1";
	windowParams.Size = { 640, 480 };
	auto window1 = myCoreSetup->WindowManager->NewWindow(windowParams);
	myWindow1 = myCoreSetup->GraphicsManager->CreateRenderTextureForWindow(*window1);
	window1->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		myWindow1.reset();
		});

	windowParams.Title = "Window 2";
	windowParams.Size = { 640, 480 };
	auto window2 = myCoreSetup->WindowManager->NewWindow(windowParams);
	myWindow2 = myCoreSetup->GraphicsManager->CreateRenderTextureForWindow(*window2);
	window2->Closed.Connect(nullptr, [&](RoseGold::Core::Platform::Window&) {
		myWindow2.reset();
		});
}

void ExampleGame::OnStart_DefineMeshes()
{
	ZoneScoped;
	{
		myColoredTriangle = ColoredMesh(myCoreSetup->GraphicsManager.get());
		std::vector<ColoredVertex> triangle;
		triangle.emplace_back(ColoredVertex{ { -1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f } });
		triangle.emplace_back(ColoredVertex{ {  0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f } });
		triangle.emplace_back(ColoredVertex{ {  1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f } });
		myColoredTriangle.SetVertices(triangle);
	}

	{
		myColoredPlane = ColoredMesh(myCoreSetup->GraphicsManager.get());
		std::vector<ColoredVertex> plane;

		plane.emplace_back(ColoredVertex{ { -1.f, 0.f,-1.f }, { 0.f, 0.f, 1.f, 1.f } });
		plane.emplace_back(ColoredVertex{ { -1.f, 0.f, 1.f }, { 0.f, 1.f, 1.f, 1.f } });
		plane.emplace_back(ColoredVertex{ {  1.f, 0.f, 1.f }, { 1.f, 1.f, 1.f, 1.f } });

		plane.emplace_back(ColoredVertex{ {  1.f, 0.f, 1.f }, { 1.f, 1.f, 1.f, 1.f } });
		plane.emplace_back(ColoredVertex{ {  1.f, 0.f,-1.f }, { 1.f, 0.f, 1.f, 1.f } });
		plane.emplace_back(ColoredVertex{ { -1.f, 0.f,-1.f }, { 0.f, 0.f, 1.f, 1.f } });

		myColoredPlane.SetVertices(plane);
	}

	{
		myTexturedPyramid = TexturedMesh(myCoreSetup->GraphicsManager.get());
		std::vector<TexturedVertex> pyramid;

		const TexturedVertex frontLeft({ -1.f, 0.f,-1.f }, { 0.f, 1.f });
		const TexturedVertex backLeft({ -1.f, 0.f, 1.f }, { 0.f, 0.f });
		const TexturedVertex frontRight({ 1.f, 0.f,-1.f }, { 1.f, 1.f });
		const TexturedVertex backRight({ 1.f, 0.f, 1.f }, { 1.f, 0.f });

		const TexturedVertex top({ 0.f, 1.f, 0.f }, { 0.5f, 0.5f });

		// Bottom
		pyramid.push_back(frontLeft);
		pyramid.push_back(backRight);
		pyramid.push_back(backLeft);

		pyramid.push_back(backRight);
		pyramid.push_back(frontLeft);
		pyramid.push_back(frontRight);

		// Left
		pyramid.push_back(frontLeft);
		pyramid.push_back(backLeft);
		pyramid.push_back(top);

		// Right
		pyramid.push_back(backRight);
		pyramid.push_back(frontRight);
		pyramid.push_back(top);

		// Front
		pyramid.push_back(frontRight);
		pyramid.push_back(frontLeft);
		pyramid.push_back(top);

		// Back
		pyramid.push_back(backLeft);
		pyramid.push_back(backRight);
		pyramid.push_back(top);

		myTexturedPyramid.SetVertices(pyramid);
	}
}

void ExampleGame::OnStart_SetupPipelineStates()
{
	ZoneScoped;
	RoseGold::Debug::Assert(
		myWindow1->GetDescriptor().ColorGraphicsFormat == myWindow2->GetDescriptor().ColorGraphicsFormat &&
		myWindow1->GetDescriptor().DepthStencilFormat == myWindow2->GetDescriptor().DepthStencilFormat,
		"Code assumes both window render-targets have the same formats. If this is not the case, each render-target needs a special pipeline state for each used shader combination."
	);

	std::shared_ptr<RoseGold::Core::Graphics::Shader> coloredVertexShader = myCoreSetup->GraphicsManager->CreateShader(
		"example/CoreOnly_MeshVertex.hlsl",
		RoseGold::Core::Graphics::Shader::Type::Vertex,
		"ColoredMesh");

	// Same file as ColoredMesh, but a different entrypoint.
	std::shared_ptr<RoseGold::Core::Graphics::Shader> texturedVertexShader = myCoreSetup->GraphicsManager->CreateShader(
		"example/CoreOnly_MeshVertex.hlsl",
		RoseGold::Core::Graphics::Shader::Type::Vertex,
		"TexturedMesh");

	// Only one pixel-shader as the vertex-shaders have the same output.
	std::shared_ptr<RoseGold::Core::Graphics::Shader> pixelShader = myCoreSetup->GraphicsManager->CreateShader(
		"example/CoreOnly_MeshPixel.hlsl",
		RoseGold::Core::Graphics::Shader::Type::Pixel);

	{
		RoseGold::Core::Graphics::PipelineStateDescription pipelineState;
		pipelineState.InputLayout = ColoredVertex::GetInputLayout();
		pipelineState.VertexShader = coloredVertexShader;
		pipelineState.PixelShader = pixelShader;
		pipelineState.OutputFormats = { myWindow1->GetDescriptor().ColorGraphicsFormat };
		pipelineState.DepthTargetFormat = myWindow1->GetDescriptor().DepthStencilFormat;
		myColoredMeshPipelineState = myCoreSetup->GraphicsManager->CreateOrGetPipelineState(pipelineState);
	}

	{
		RoseGold::Core::Graphics::PipelineStateDescription pipelineState;
		pipelineState.InputLayout = TexturedVertex::GetInputLayout();
		pipelineState.VertexShader = texturedVertexShader;
		pipelineState.PixelShader = pixelShader;
		pipelineState.OutputFormats = { myWindow1->GetDescriptor().ColorGraphicsFormat };
		pipelineState.DepthTargetFormat = myWindow1->GetDescriptor().DepthStencilFormat;
		myTexturedMeshPipelineState = myCoreSetup->GraphicsManager->CreateOrGetPipelineState(pipelineState);
	}
}

void ExampleGame::OnStart_CreateMVPBuffers()
{
	ZoneScoped;
	using namespace RoseGold::Core::Graphics;
	auto makeCameraConstants = [&]() { return myCoreSetup->GraphicsManager->CreateGraphicsBuffer(GraphicsBuffer::Target::Constant, 1, sizeof(CameraConstants)); };

	myRT1PyramidConstants = makeCameraConstants();
	myRT1TriangleConstants = makeCameraConstants();
	myRT1PlaneConstants = makeCameraConstants();

	myRT2PyramidConstants = makeCameraConstants();
	myRT2TriangleConstants = makeCameraConstants();
	myRT2PlaneConstants = makeCameraConstants();
}
