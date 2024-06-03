// Filter "Chart"
#include "stdafx.hpp"
#include "ChartTestWindow.hpp"
#include "ChartData.hpp"

#include "Editor_FileDialog.hpp"
#include "Editor_GUI.hpp"

using namespace RoseGold::EditorGUI;
using namespace std::chrono_literals;

void TimeText(std::chrono::microseconds aTime)
{
	std::int64_t lengthSeconds = std::chrono::duration_cast<std::chrono::seconds>(aTime).count();
	std::int64_t lengthMinutes = lengthSeconds / 60;
	lengthSeconds -= lengthMinutes * 60;
	Text::Formatted("%02im %02is", lengthMinutes, lengthSeconds);
}

void ChartTestWindow::ImGui()
{
	Window::WindowData windowData;
	std::string title = std::format("Chart - {}###ChartTestWindow", myCurrentSong);
	windowData.Name = title.c_str();
	if (Window::WindowScope window{ windowData })
	{
		switch (myState)
		{
		case State::SongList:
			ImGui_ChartList();
			break;
		case State::Player:
			ImGui_Player();
			break;
		}
	}
}

void ChartTestWindow::ImGui_ChartList()
{
	Text::Unformatted(mySongsDirectory.string().c_str());
	Layout::SameLine();

	if (Widget::Button("..."))
	{
		RoseGold::Editor::FolderBrowserDialog pickFolder;
		auto pickedFolder = pickFolder.GetSingle();
		if (pickedFolder.has_value())
			mySongsDirectory = pickedFolder.value();
	}

	Layout::NewLine();

	if (Widget::Button("Refresh"))
		RefreshSongList();

	if (Table::TableScope songTable{ "Song table", 3 })
	{
		songTable.Setup_Column("Info");
		songTable.Setup_Column("Details");
		songTable.Setup_HeadersRow();

		for (const auto& it : myChartInfos)
		{
			IDScope idScope((void*)it.first.c_str());

			const ChartInfo::SongInfo& songInfo = it.second->GetSongInfo();

			songTable.NextColumn();
			Text::Unformatted(songInfo.Title.c_str());
			Text::Formatted("by %s", songInfo.Artist.c_str());
			Text::Formatted("%s (%i)", songInfo.Album.c_str(), songInfo.Year);

			TimeText(songInfo.SongLength);

			songTable.NextColumn();

			auto difficultyWidget = [&](ChartTrackType aTrack, const char* aTitle)
				{
					const int difficulty = it.second->GetDifficulty(aTrack);
					if (difficulty == -1)
						return;

					const float max = static_cast<float>(ChartInfo::MaxDifficulty);
					const float current = std::clamp(static_cast<float>(difficulty), 0.f, max);
					Widget::ProgressBar(current / max, { 0, 0 }, aTitle);
				};

			difficultyWidget(ChartTrackType::LeadGuitar, "Lead guitar");
			difficultyWidget(ChartTrackType::RhythmGuitar, "Rhythm guitar");
			difficultyWidget(ChartTrackType::BassGuitar, "Bass guitar");
			difficultyWidget(ChartTrackType::Drums, "Drums");
			difficultyWidget(ChartTrackType::Vocal_Main, "Vocals");
			difficultyWidget(ChartTrackType::Vocal_Harmony, "Harmony vocals");

			songTable.NextColumn();

			if (Widget::Button("Select"))
				SelectSong(it.first);
		}
	}
}

void ChartTestWindow::ImGui_Player()
{
	if (Widget::Button("Back to song list"))
		ReturnToSongList();

	if (!myChartPlayer.IsPlaying())
	{
		if (Widget::Button("Play"))
			myChartPlayer.Play();
	}
	else
	{
		if (Widget::Button("Stop"))
			myChartPlayer.Stop();
	}

	Text::Formatted("PPQ: %u", myChartData.GetTicksPerQuarterNote());
	Text::Formatted("BPM: %f", myChartData.GetBPMAt(myChartPlayer.GetTick()));

	TimeText(myChartData.TicksToTime(myChartPlayer.GetTick()));
	Layout::SameLine();
	Text::Unformatted(" / ");
	Layout::SameLine();
	TimeText(myChartData.TicksToTime(myChartData.GetLength()));
}

void ChartTestWindow::RefreshSongList()
{
	if (!std::filesystem::exists(mySongsDirectory) || !std::filesystem::is_directory(mySongsDirectory))
		return;

	myChartInfos.clear();

	for (const auto& entry : std::filesystem::directory_iterator(mySongsDirectory))
	{
		const std::filesystem::path songIni = entry.path() / "song.ini";

		if (!std::filesystem::exists(songIni))
			continue;

		std::unique_ptr<ChartInfo> chartInfo = std::make_unique<ChartInfo>();
		chartInfo->Load(songIni);
		myChartInfos[songIni] = std::move(chartInfo);
	}
}

void ChartTestWindow::SelectSong(const std::filesystem::path& aSong)
{
	const std::unique_ptr<ChartInfo>& chart = myChartInfos.at(aSong);
	myCurrentSong = chart->GetSongInfo().Title;
	myChartData.LoadMidi(aSong.parent_path() / "notes.mid");
	myState = State::Player;
}

void ChartTestWindow::ReturnToSongList()
{
	myCurrentSong.clear();
	myChartPlayer.Stop();
	myState = State::SongList;
}
