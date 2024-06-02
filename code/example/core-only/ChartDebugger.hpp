// Filter "Chart"
#pragma once

#include "ChartData.hpp"

#include <filesystem>

class ChartInspector
{
public:
	void ImGui();

private:
	void ImGui_Information();

	ChartData myChartData;
};
