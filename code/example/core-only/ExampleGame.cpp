#include "stdafx.hpp"

#include "ExampleGame.hpp"

#include <Common_Diagnostics.hpp>

ExampleGame::ExampleGame()
	: myChartTestWindow(myChartPlayer)
	, myChartRenderer(myChartPlayer)
{ }

void ExampleGame::OnStart()
{
	ZoneScoped;

	OnStart_SetupWindows();

	myChartRenderer.SetupResources(GetGraphicsAPI(), myWindow1->GetDescriptor().ColorGraphicsFormat, myWindow1->GetDescriptor().DepthStencilFormat);
}

void ExampleGame::OnLoop()
{
	if (GetWindowManager().GetWindows().empty())
		ShutDown();

	ZoneScoped;

	using namespace RoseGold::Core;
	FrameContext& frameContext = GetGraphicsAPI().GetCurrentFrameContext();

	if (myWindow1)
	{
		{
			CONTEXT_ZONE(frameContext, "Clear");
			frameContext.ClearColor(myWindow1, RoseGold::Color::Predefined::CornflowerBlue);
			frameContext.ClearDepth(myWindow1, 1.f, 0);
		}

		myChartRenderer.Render(frameContext, myWindow1);
	}
}

void ExampleGame::OnImGui()
{
	myChartTestWindow.ImGui();
}

void ExampleGame::OnExit()
{
	ZoneScoped;
}

void ExampleGame::OnStart_SetupWindows()
{
	ZoneScoped;
	RoseGold::Core::WindowManager::CreationParameters windowParams;
	windowParams.Title = "Window 1";
	windowParams.Size = { 640, 480 };
	auto window1 = GetWindowManager().NewWindow(windowParams);
	myWindow1 = GetGraphicsAPI().GetResourceManager().CreateRenderTextureForWindow(*window1);
	window1->Closed.Connect(nullptr, [&](RoseGold::Core::Window&) {
		myWindow1.reset();
		});

	GetEngine().InitializeImGui(*window1, myWindow1);
}

//void ExampleGame::OnStart_DefineMeshes()
//{
//	ZoneScoped;
//	{
//		myColoredTriangle = ColoredMesh(&GetGraphicsAPI());
//		myColoredTriangle.SetName(L"Triangle");
//		std::vector<ColoredVertex> triangle;
//		triangle.emplace_back(ColoredVertex{ { -1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f } });
//		triangle.emplace_back(ColoredVertex{ {  0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f } });
//		triangle.emplace_back(ColoredVertex{ {  1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f } });
//		myColoredTriangle.SetVertices(triangle);
//	}
//
//	{
//		myColoredPlane = ColoredMesh(&GetGraphicsAPI());
//		myColoredPlane.SetName(L"Plane");
//		std::vector<ColoredVertex> plane;
//
//		plane.emplace_back(ColoredVertex{ { -1.f, 0.f,-1.f }, { 0.f, 0.f, 1.f, 1.f } });
//		plane.emplace_back(ColoredVertex{ { -1.f, 0.f, 1.f }, { 0.f, 1.f, 1.f, 1.f } });
//		plane.emplace_back(ColoredVertex{ {  1.f, 0.f, 1.f }, { 1.f, 1.f, 1.f, 1.f } });
//
//		plane.emplace_back(ColoredVertex{ {  1.f, 0.f, 1.f }, { 1.f, 1.f, 1.f, 1.f } });
//		plane.emplace_back(ColoredVertex{ {  1.f, 0.f,-1.f }, { 1.f, 0.f, 1.f, 1.f } });
//		plane.emplace_back(ColoredVertex{ { -1.f, 0.f,-1.f }, { 0.f, 0.f, 1.f, 1.f } });
//
//		myColoredPlane.SetVertices(plane);
//	}
//
//	{
//		myTexturedPyramid = TexturedMesh(&GetGraphicsAPI());
//		myTexturedPyramid.SetName(L"Pyramid");
//		std::vector<TexturedVertex> pyramid;
//
//		const TexturedVertex frontLeft({ -1.f, 0.f,-1.f }, { 0.f, 1.f });
//		const TexturedVertex backLeft({ -1.f, 0.f, 1.f }, { 0.f, 0.f });
//		const TexturedVertex frontRight({ 1.f, 0.f,-1.f }, { 1.f, 1.f });
//		const TexturedVertex backRight({ 1.f, 0.f, 1.f }, { 1.f, 0.f });
//
//		const TexturedVertex top({ 0.f, 1.f, 0.f }, { 0.5f, 0.5f });
//
//		// Bottom
//		pyramid.push_back(frontLeft);
//		pyramid.push_back(backRight);
//		pyramid.push_back(backLeft);
//
//		pyramid.push_back(backRight);
//		pyramid.push_back(frontLeft);
//		pyramid.push_back(frontRight);
//
//		// Left
//		pyramid.push_back(frontLeft);
//		pyramid.push_back(backLeft);
//		pyramid.push_back(top);
//
//		// Right
//		pyramid.push_back(backRight);
//		pyramid.push_back(frontRight);
//		pyramid.push_back(top);
//
//		// Front
//		pyramid.push_back(frontRight);
//		pyramid.push_back(frontLeft);
//		pyramid.push_back(top);
//
//		// Back
//		pyramid.push_back(backLeft);
//		pyramid.push_back(backRight);
//		pyramid.push_back(top);
//
//		myTexturedPyramid.SetVertices(pyramid);
//	}
//}

//void ExampleGame::OnStart_SetupPipelineStates()
//{
//	ZoneScoped;
//	if (myWindow1 && myWindow2)
//	{
//		RoseGold::Debug::Assert(
//			myWindow1->GetDescriptor().ColorGraphicsFormat == myWindow2->GetDescriptor().ColorGraphicsFormat &&
//			myWindow1->GetDescriptor().DepthStencilFormat == myWindow2->GetDescriptor().DepthStencilFormat,
//			"Code assumes both window render-targets have the same formats. If this is not the case, each render-target needs a special pipeline state for each used shader combination."
//		);
//	}
//
//	std::shared_ptr<RoseGold::Core::Shader> coloredVertexShader = GetGraphicsAPI().GetResourceManager().CreateShader(
//		"example/CoreOnly_MeshVertex.hlsl",
//		RoseGold::Core::Shader::Type::Vertex,
//		"ColoredMesh");
//
//	// Same file as ColoredMesh, but a different entrypoint.
//	std::shared_ptr<RoseGold::Core::Shader> texturedVertexShader = GetGraphicsAPI().GetResourceManager().CreateShader(
//		"example/CoreOnly_MeshVertex.hlsl",
//		RoseGold::Core::Shader::Type::Vertex,
//		"TexturedMesh");
//
//	// Only one pixel-shader as the vertex-shaders have the same output.
//	std::shared_ptr<RoseGold::Core::Shader> pixelShader = GetGraphicsAPI().GetResourceManager().CreateShader(
//		"example/CoreOnly_MeshPixel.hlsl",
//		RoseGold::Core::Shader::Type::Pixel,
//		"main");
//
//	{
//		RoseGold::Core::PipelineStateDescription pipelineState;
//		pipelineState.InputLayout = ColoredVertex::GetInputLayout();
//		pipelineState.VertexShader = coloredVertexShader;
//		pipelineState.PixelShader = pixelShader;
//		pipelineState.OutputFormats = { myWindow1->GetDescriptor().ColorGraphicsFormat };
//		pipelineState.DepthTargetFormat = myWindow1->GetDescriptor().DepthStencilFormat;
//		myColoredMeshPipelineState = GetGraphicsAPI().GetResourceManager().CreateOrGetPipelineState(pipelineState);
//	}
//
//	{
//		RoseGold::Core::PipelineStateDescription pipelineState;
//		pipelineState.InputLayout = TexturedVertex::GetInputLayout();
//		pipelineState.VertexShader = texturedVertexShader;
//		pipelineState.PixelShader = pixelShader;
//		pipelineState.OutputFormats = { myWindow1->GetDescriptor().ColorGraphicsFormat };
//		pipelineState.DepthTargetFormat = myWindow1->GetDescriptor().DepthStencilFormat;
//		myTexturedMeshPipelineState = GetGraphicsAPI().GetResourceManager().CreateOrGetPipelineState(pipelineState);
//	}
//}
