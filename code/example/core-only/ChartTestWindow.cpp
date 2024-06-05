// Filter "Chart"
#include "stdafx.hpp"
#include "ChartTestWindow.hpp"
#include "ChartData.hpp"

#include "Editor_FileDialog.hpp"
#include "Editor_GUI.hpp"

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
			ImGui::TextUnformatted(std::format("{:%M:%S}", std::chrono::round<std::chrono::seconds>(songInfo.SongLength)).c_str());

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

	ImGui_Player_PlayControls();
#endif
}

void ChartTestWindow::ImGui_Player_PlayControls()
{
#if IS_IMGUI_ENABLED
	ImGui::Text("BPM: %f", myChartData.GetBPMAt(myChartPlayer.GetPlayhead()));

	const ChartPlayer::State playerState = myChartPlayer.GetState();
	ImGui::BeginDisabled(playerState == ChartPlayer::State::Seeking);

	ImGui::BeginDisabled(playerState == ChartPlayer::State::Playing);
	if (ImGui::Button("Play"))
		myChartPlayer.Play();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(playerState == ChartPlayer::State::Paused);
	if (ImGui::Button("Pause"))
		myChartPlayer.Pause();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(playerState == ChartPlayer::State::Stopped);
	if (ImGui::Button("Stop"))
		myChartPlayer.Stop();
	ImGui::EndDisabled();

	ImGui::EndDisabled();

	const auto playhead = myChartPlayer.GetPlayhead();
	const auto duration = myChartData.GetDuration();
	const auto playheadSeconds = std::chrono::round<std::chrono::seconds>(playhead);
	const auto durationSeconds = std::chrono::round<std::chrono::seconds>(duration);
	int playheadMicroseconds = static_cast<int>(playhead.count());
	if (ImGui::SliderInt("###SeekBar", &playheadMicroseconds, 0, static_cast<int>(duration.count()), std::format("{:%M:%S} / {:%M:%S}", playheadSeconds, durationSeconds).c_str()))
	{
		if (playerState == ChartPlayer::State::Stopped)
			myChartPlayer.Pause();
		myChartPlayer.Seek(std::chrono::microseconds(playheadMicroseconds));
	}

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
