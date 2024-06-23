#pragma once

#include <Engine_Game.hpp>

#include <Graphics_Manager.hpp>

#include "ChartRenderer.hpp"
#include "ChartTestWindow.hpp"
#include "Mesh.hpp"

#include <memory>
#include <vector>

class ExampleGame : public RoseGold::Game
{
public:
	ExampleGame();

	void OnStart() override;
	void OnLoop() override;
	void OnImGui() override;
	void OnExit() override;

private:
	void OnStart_SetupWindows();

	std::shared_ptr<RoseGold::Core::RenderTexture> myWindow1;

	ChartTestWindow myChartTestWindow;
	ChartRenderer myChartRenderer;
	ChartPlayer myChartPlayer;
};
