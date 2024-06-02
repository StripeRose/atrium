// Filter "Chart"
#pragma once

#include "ChartData.hpp"
#include "ChartPlayback.hpp"

#include <filesystem>
#include <map>

class ChartTestWindow
{
public:
	void ImGui();

private:
	void ImGui_ChartList();

	void ImGui_Player();

	void RefreshSongList();
	void SelectSong(const std::filesystem::path& aSong);
	void ReturnToSongList();

	enum class State { SongList, Player };

	State myState = State::SongList;

	std::filesystem::path mySongsDirectory;
	std::map<std::filesystem::path, std::unique_ptr<ChartInfo>> myChartInfos;

	std::string myCurrentSong;

	ChartData myChartData;
	ChartPlayer myChartPlayer;
};
