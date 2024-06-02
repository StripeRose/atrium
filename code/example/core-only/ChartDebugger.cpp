// Filter "Chart"
#include "stdafx.hpp"
#include "ChartDebugger.hpp"
#include "ChartData.hpp"

#include "Editor_FileDialog.hpp"
#include "Editor_GUI.hpp"

using namespace RoseGold::EditorGUI;

void ChartInspector::ImGui()
{
	Window::WindowData windowData;
	windowData.Name = "Chart debugger";
	if (Window::WindowScope window{ windowData })
	{
		if (Widget::Button("Load"))
		{
			RoseGold::Editor::OpenFileDialog openFile;
			RoseGold::Editor::FileDialogFilter filter[] = {
				{"Song.ini", "song.ini"}
			};
			openFile.SetFilter(filter);

			auto iniPath = openFile.GetSingle();
			if (iniPath.has_value())
				myChartData.Load(iniPath.value().parent_path());
		}

		ImGui_Information();
	}
}

void ChartInspector::ImGui_Information()
{
	const ChartInfo::SongInfo& songInfo = myChartData.GetInfo().GetSongInfo();

	Text::Unformatted(songInfo.Title.c_str());
	Text::Formatted("by %s", songInfo.Artist.c_str());
	Text::Formatted("%s (%i)", songInfo.Album.c_str(), songInfo.Year);

	std::int64_t lengthSeconds = songInfo.SongLength.count() / 1000;
	std::int64_t lengthMinutes = lengthSeconds / 60;
	lengthSeconds -= lengthMinutes * 60;
	Text::Formatted("%im %is", lengthMinutes, lengthSeconds);
}
