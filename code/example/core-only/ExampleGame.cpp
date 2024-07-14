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

	myChartRenderer.SetupResources(GetGraphicsAPI(), myWindow1->GetDescriptor().ColorGraphicsFormat);
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
			frameContext.ClearColor(myWindow1, RoseGold::Color::Predefined::Black);
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
