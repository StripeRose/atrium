// Filter "Chart"
#pragma once

#include "ChartData.hpp"

#include <filesystem>
#include <map>

class ChartTestWindow
{
public:
	void ImGui();

private:
	void ImGui_ChartList();

	void RefreshSongList();

	enum class State { SongList };

	State myState = State::SongList;

	std::filesystem::path mySongsDirectory;
	std::map<std::filesystem::path, std::unique_ptr<ChartInfo>> myChartInfos;
};
