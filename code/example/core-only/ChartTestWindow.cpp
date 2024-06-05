// Filter "Chart"
#include "stdafx.hpp"
#include "ChartTestWindow.hpp"
#include "ChartData.hpp"

#include "Editor_FileDialog.hpp"
#include "Editor_GUI.hpp"

#if IS_IMGUI_ENABLED
void TimeText(std::chrono::microseconds aTime)
{
	std::int64_t lengthSeconds = std::chrono::duration_cast<std::chrono::seconds>(aTime).count();
	std::int64_t lengthMinutes = lengthSeconds / 60;
	lengthSeconds -= lengthMinutes * 60;
	ImGui::Text("%02im %02is", lengthMinutes, lengthSeconds);
}
#endif

void ChartTestWindow::ImGui()
{
#if IS_IMGUI_ENABLED
	std::string title = std::format("Chart - {}###ChartTestWindow", myCurrentSong);
	ImGui::Begin(title.c_str());
	switch (myState)
	{
	case State::SongList:
		ImGui_ChartList();
		break;
	case State::Player:
		ImGui_Player();
		break;
	}
	ImGui::End();
#endif
}

void ChartTestWindow::ImGui_ChartList()
{
#if IS_IMGUI_ENABLED
	ImGui::TextUnformatted(mySongsDirectory.string().c_str());
	ImGui::SameLine();

	if (ImGui::Button("..."))
	{
		RoseGold::Editor::FolderBrowserDialog pickFolder;
		auto pickedFolder = pickFolder.GetSingle();
		if (pickedFolder.has_value())
			mySongsDirectory = pickedFolder.value();
	}

	ImGui::NewLine();

	if (ImGui::Button("Refresh"))
		RefreshSongList();

	if (ImGui::BeginTable("Song table", 3))
	{
		ImGui::TableSetupColumn("Info");
		ImGui::TableSetupColumn("Details");
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();

		for (const auto& it : myChartInfos)
		{
			ImGui::TableNextRow();
			ImGui::PushID(it.first.c_str());

			const ChartInfo::SongInfo& songInfo = it.second->GetSongInfo();

			ImGui::TableNextColumn();
			ImGui::TextUnformatted(songInfo.Title.c_str());
			ImGui::Text("by %s", songInfo.Artist.c_str());
			ImGui::Text("%s (%i)", songInfo.Album.c_str(), songInfo.Year);

			TimeText(songInfo.SongLength);

			ImGui::TableNextColumn();

			auto difficultyWidget = [&](ChartTrackType aTrack, const char* aTitle)
				{
					const int difficulty = it.second->GetDifficulty(aTrack);
					if (difficulty == -1)
						return;

					const float max = static_cast<float>(ChartInfo::MaxDifficulty);
					const float current = std::clamp(static_cast<float>(difficulty), 0.f, max);
					ImGui::ProgressBar(current / max, { 0, 0 }, aTitle);
				};

			difficultyWidget(ChartTrackType::LeadGuitar, "Lead guitar");
			difficultyWidget(ChartTrackType::RhythmGuitar, "Rhythm guitar");
			difficultyWidget(ChartTrackType::BassGuitar, "Bass guitar");
			difficultyWidget(ChartTrackType::Drums, "Drums");
			difficultyWidget(ChartTrackType::Vocal_Main, "Vocals");
			difficultyWidget(ChartTrackType::Vocal_Harmony, "Harmony vocals");

			ImGui::TableNextColumn();

			if (ImGui::Button("Select"))
				SelectSong(it.first);

			ImGui::PopID();
		}

		ImGui::EndTable();
	}
#endif
}

void ChartTestWindow::ImGui_Player()
{
#if IS_IMGUI_ENABLED
	myChartPlayer.Update();

	if (ImGui::Button("Back to song list"))
		ReturnToSongList();

	if (!myChartPlayer.IsPlaying())
	{
		if (ImGui::Button("Play"))
			myChartPlayer.Play();
	}
	else
	{
		if (ImGui::Button("Stop"))
			myChartPlayer.Stop();
	}

	ImGui::Text("BPM: %f", myChartData.GetBPMAt(myChartPlayer.GetPlayhead()));
	TimeText(myChartPlayer.GetPlayhead());
	ImGui::SameLine();
	ImGui::TextUnformatted(" / ");
	ImGui::SameLine();
	TimeText(myChartData.GetDuration());
#endif
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
	myChartPlayer.SetChartData(myChartData);
	myState = State::Player;
}

void ChartTestWindow::ReturnToSongList()
{
	myCurrentSong.clear();
	myChartPlayer.Stop();
	myState = State::SongList;
}
