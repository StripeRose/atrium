// Filter "Chart"
#include "stdafx.hpp"
#include "ChartData.hpp"

#include "MidiDecoder.hpp"

#include "Common_Diagnostics.hpp"

void ChartInfo::Load(const std::filesystem::path& aSongIni)
{
	myIni.ReadFromFile(aSongIni);
	const RoseCommon::Ini::Section& song = myIni.GetSection("song");

	mySongInfo.Title = song.Get("name");
	mySongInfo.Artist = song.Get("artist");
	mySongInfo.Album = song.Get("album");
	mySongInfo.Genre = song.Get("genre");
	mySongInfo.Year = song.Get<unsigned int>("year");
	mySongInfo.SongLength = std::chrono::milliseconds(song.Get<std::int64_t>("song_length"));

	myDifficulties[ChartTrackType::LeadGuitar] = song.Has("diff_guitar") ? song.Get<int>("diff_guitar") : -1;
	myDifficulties[ChartTrackType::RhythmGuitar] = song.Has("diff_rhythm") ? song.Get<int>("diff_rhythm") : -1;
	myDifficulties[ChartTrackType::BassGuitar] = song.Has("diff_bass") ? song.Get<int>("diff_bass") : -1;
	myDifficulties[ChartTrackType::Drums] = song.Has("diff_drums") ? song.Get<int>("diff_drums") : -1;
	myDifficulties[ChartTrackType::Vocal_Main] = song.Has("diff_vocals") ? song.Get<int>("diff_vocals") : -1;
	myDifficulties[ChartTrackType::Vocal_Harmony] = song.Has("diff_vocals_harm") ? song.Get<int>("diff_vocals_harm") : -1;
}

float ChartData::GetBPMAt(std::uint32_t aTick) const
{
	return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::minutes(1)).count()) / static_cast<float>(GetTempoAt(aTick));
}

void ChartData::LoadMidi(const std::filesystem::path& aMidi)
{
	MidiDecoder decoder;

	MidiDecoder::FormatType formatType = MidiDecoder::FormatType::SingleTrack;

	ChartTrack* currentTrack = nullptr;

	std::map<std::uint8_t, std::uint32_t> myPartialNotes;

	using namespace RoseCommon;

	decoder.OnNewTrack.Connect(this, [&]()
		{
			currentTrack = nullptr;
			myPartialNotes.clear();
		}
	);

	decoder.OnTrackName.Connect(this, [&](std::uint32_t, const std::string& aText)
		{
			if (aText.starts_with("PART "))
			{
				std::unique_ptr<ChartTrack> track = ChartTrack::CreateTrackByName(aText.substr(5));
				if (!track)
					return;

				if (myTracks.contains(track->GetType()))
					return;

				currentTrack = track.get();
				myTracks[track->GetType()] = std::move(track);
			}
		}
	);

	decoder.OnNote.Connect(this, [&](std::uint32_t aTick, std::uint8_t /*aChannel*/, std::uint8_t aNote, std::uint8_t aVelocity)
		{
			if (!currentTrack)
				return;

			if (myPartialNotes.contains(aNote))
			{
				currentTrack->AddNote(aNote, myPartialNotes.at(aNote), aTick);
				myPartialNotes.erase(aNote);
			}

			// New note to start
			if (aVelocity > 0)
				myPartialNotes[aNote] = aTick;
		}
	);

	decoder.OnSysEx.Connect(this, [&](std::uint32_t aTick, const std::span<std::uint8_t>& someData)
		{
			if (currentTrack)
				currentTrack->AddSysEx(aTick, someData);
		}
	);

	decoder.OnText.Connect(this, [&](std::uint32_t aTick, const std::string& aText)
		{
			std::string::const_iterator nameStart, nameEnd;
			if (aText.starts_with("[section"))
			{
				nameStart = aText.cbegin() + 9;
				nameEnd = aText.cend() - 1;
			}
			else if (aText.starts_with("[prc_"))
			{
				nameStart = aText.cbegin() + 5;
				nameEnd = aText.cend() - 1;
			}

			mySections.emplace_back(aTick, std::string(nameStart, nameEnd));
		}
	);

	decoder.OnLyric.Connect(this, [&](std::uint32_t aTick, const std::string& aText)
		{
			if (currentTrack)
				currentTrack->AddLyric(aTick, aText);
		}
	);

	decoder.OnTrackEnd.Connect(this, [&](std::uint32_t aTick)
		{
			myChartEndInTicks = std::max(myChartEndInTicks, aTick);
			currentTrack = nullptr;
		}
	);

	decoder.OnSetTempo.Connect(this, [&](std::uint32_t aTick, std::uint32_t aTempo)
		{
			myTempos.emplace_back(aTick, aTempo);
		}
	);

	decoder.OnTimeSignature.Connect(this, [&](std::uint32_t aTick, std::uint8_t aNumerator, std::uint8_t aDenominator, std::uint8_t aClock, std::uint8_t aBase)
		{
			myTimeSignatures.emplace_back(aTick, TimeSignature{ aNumerator, aDenominator, aClock, aBase });
		}
	);

	decoder.ProcessFile(aMidi, formatType, myTicksPerQuarterNote);
}

std::chrono::microseconds ChartData::TicksToTime(std::uint32_t aTick) const
{
	std::chrono::microseconds totalTimeAtTick(0);

	for (auto tempoSection = myTempos.begin(); tempoSection != myTempos.end(); ++tempoSection)
	{
		auto nextTempoSection = tempoSection + 1;
		const std::chrono::microseconds microsecondsPerTick(tempoSection->second / myTicksPerQuarterNote);

		if (nextTempoSection == myTempos.end() || (tempoSection->first >= aTick && aTick < nextTempoSection->first))
		{
			std::int64_t sectionTickCount = (aTick - tempoSection->first);
			totalTimeAtTick += std::chrono::microseconds(sectionTickCount * microsecondsPerTick);
			break;
		}
		else
		{
			std::int64_t sectionTickCount = (nextTempoSection->first - tempoSection->first);
			totalTimeAtTick += std::chrono::microseconds(sectionTickCount * microsecondsPerTick);
		}
	}

	return totalTimeAtTick;
}

std::uint32_t ChartData::GetTempoAt(std::uint32_t aTick) const
{
	std::uint32_t currentTempo = 0;
	for (const auto& it : myTempos)
	{
		if (it.first <= aTick)
			currentTempo = it.second;
		else
			break;
	}

	return currentTempo;
}
