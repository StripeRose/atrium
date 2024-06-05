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

float ChartData::GetBPMAt(std::chrono::microseconds aTime) const
{
	return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::minutes(1)).count()) / static_cast<float>(GetTempoAt(aTime));
}

std::chrono::microseconds ChartData::GetDuration() const
{
	if (!myTempos.empty())
		return myTempos.back().first;
	else
		return std::chrono::microseconds(0);
}

void ChartData::LoadMidi(const std::filesystem::path& aMidi)
{
	myTempos.clear();
	mySections.clear();
	myTimeSignatures.clear();
	myTracks.clear();

	MidiDecoder decoder;

	std::uint16_t ticksPerQuarterNote(0);
	MidiDecoder::FormatType formatType = MidiDecoder::FormatType::SingleTrack;

	std::unique_ptr<ChartTrack> currentTrack;
	ChartTrackLoadData currentTrackLoadData;
	std::uint32_t currentTrackTempoStartInTicks(0);
	static constexpr std::uint32_t DefaultMidiTempo = 120 * static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::minutes(1)).count());

	auto ticksToTime = [&](std::uint32_t aTick) -> std::chrono::microseconds
		{
			std::chrono::microseconds currentTempoStart{ 0 };
			std::uint32_t currentTempo = DefaultMidiTempo;
			if (!myTempos.empty())
			{
				currentTempoStart = myTempos.back().first;
				currentTempo = myTempos.back().second;
			}

			const std::chrono::microseconds microsecondsPerTick(currentTempo / ticksPerQuarterNote);
			std::int64_t sectionTickCount = aTick;
			sectionTickCount -= currentTrackTempoStartInTicks;
			return currentTempoStart + std::chrono::microseconds(sectionTickCount * microsecondsPerTick);
		};

	decoder.OnNewTrack.Connect(this, [&]()
		{
			currentTrackLoadData = ChartTrackLoadData();
			currentTrack.reset();
		}
	);

	decoder.OnTrackName.Connect(this, [&](std::uint32_t, const std::string& aText)
		{
			if (aText.starts_with("PART "))
			{
				currentTrack = ChartTrack::CreateTrackByName(aText.substr(5));

				if (currentTrack && myTracks.contains(currentTrack->GetType()))
				{
					RoseGold::Debug::LogError("Duplicate track won't be processed.");
					currentTrack.reset();
				}
			}
		}
	);

	decoder.OnNote.Connect(this, [&](std::uint32_t aTick, std::uint8_t /*aChannel*/, std::uint8_t aNote, std::uint8_t aVelocity)
		{
			currentTrackLoadData.AddNote(ticksToTime(aTick), aNote, aVelocity);
		}
	);

	decoder.OnSysEx.Connect(this, [&](std::uint32_t aTick, const std::span<std::uint8_t>& someData)
		{
			currentTrackLoadData.AddSysEx(ticksToTime(aTick), someData);
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

			mySections.emplace_back(ticksToTime(aTick), std::string(nameStart, nameEnd));
		}
	);

	decoder.OnLyric.Connect(this, [&](std::uint32_t aTick, const std::string& aText)
		{
			currentTrackLoadData.AddLyric(ticksToTime(aTick), aText);
		}
	);

	decoder.OnTrackEnd.Connect(this, [&](std::uint32_t)
		{
			if (!currentTrack)
				return;

			if (currentTrack->Load(currentTrackLoadData) && !myTracks.contains(currentTrack->GetType()))
				myTracks[currentTrack->GetType()] = std::move(currentTrack);

			currentTrack.reset();
		}
	);

	decoder.OnSetTempo.Connect(this, [&](std::uint32_t aTick, std::uint32_t aTempo)
		{
			myTempos.emplace_back(ticksToTime(aTick), aTempo);
			currentTrackTempoStartInTicks = aTick;
		}
	);

	decoder.OnTimeSignature.Connect(this, [&](std::uint32_t aTick, std::uint8_t aNumerator, std::uint8_t aDenominator, std::uint8_t aClock, std::uint8_t aBase)
		{
			myTimeSignatures.emplace_back(ticksToTime(aTick), TimeSignature{ aNumerator, aDenominator, aClock, aBase });
		}
	);

	decoder.OnText.Connect(this, [&](std::uint32_t, const std::string& aText)
		{
			if (aText.find("ENHANCED_OPENS") != std::string::npos)
				currentTrackLoadData.EnhancedOpens = true;
		}
	);

	decoder.ProcessFile(aMidi, formatType, ticksPerQuarterNote);
}

std::uint32_t ChartData::GetTempoAt(std::chrono::microseconds aTime) const
{
	std::uint32_t currentTempo = 0;
	for (const auto& it : myTempos)
	{
		if (it.first <= aTime)
			currentTempo = it.second;
		else
			break;
	}

	return currentTempo;
}
