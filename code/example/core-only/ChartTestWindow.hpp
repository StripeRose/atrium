// Filter "Chart"
#pragma once

#include "ChartData.hpp"
#include "ChartPlayback.hpp"

#include "Common_Math.hpp"

#include <filesystem>
#include <map>

class ChartTestWindow
{
public:
	ChartTestWindow();
	void ImGui();

private:
#if IS_IMGUI_ENABLED
	void ImGui_ChartList();

	void ImGui_Player();
	void ImGui_Player_PlayControls();

	void ImGui_Track(ChartTrack& aTrack);
	void ImGui_Track(ChartGuitarTrack& aTrack, RoseGold::Math::Vector2 aPoint, RoseGold::Math::Vector2 aSize);
	void ImGui_Track_HitWindow(RoseGold::Math::Vector2 aPoint, RoseGold::Math::Vector2 aSize);
	void ImGui_Track_Beats(RoseGold::Math::Vector2 aPoint, RoseGold::Math::Vector2 aSize);
	void ImGui_Track_TimeSignatures();

	void ImGui_Track_Note(const ChartGuitarTrack::NoteRange& aNote, float aStartX, float anEndX, float aLaneY);
	void ImGui_Track_OpenNote(bool isHOPO, float aNoteStart, float aNoteEnd, float aTopLane, float aBottomLane);

	float ImGui_TimeToTrackPosition(float aTrackWidth, std::chrono::microseconds aTime) const;
#endif

	void RefreshSongList();
	void SelectSong(const std::filesystem::path& aSong);
	void ReturnToSongList();

	enum class State { SongList, Player };

	struct TrackSettings
	{
		ChartTrackDifficulty Difficulty = ChartTrackDifficulty::Hard;
		bool ShowOpen = true;
	};

	State myState = State::SongList;

	std::filesystem::path mySongsDirectory;
	std::map<std::filesystem::path, std::unique_ptr<ChartInfo>> myChartInfos;

	std::string myCurrentSong;

	ChartData myChartData;
	ChartPlayer myChartPlayer;
	std::map<ChartTrackType, TrackSettings> myTrackSettings;
	std::chrono::microseconds myLookAhead;
};
