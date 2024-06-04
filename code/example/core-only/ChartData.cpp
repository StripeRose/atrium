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

void ChartData::LoadMidi(const std::filesystem::path& aMidi)
{
	MidiDecoder decoder;

	MidiDecoder::FormatType formatType = MidiDecoder::FormatType::SingleTrack;

	std::unique_ptr<ChartTrack> currentTrack;
	ChartTrackLoadData currentTrackLoadData;
	std::chrono::microseconds currentTrackTempoSectionStart(0);
	std::uint32_t currentTrackTempo(0);
	std::uint16_t ticksPerQuarterNote(0);

	auto ticksToTime = [&](std::uint32_t aTick) -> std::chrono::microseconds
		{
			const std::chrono::microseconds microsecondsPerTick(currentTrackTempo / ticksPerQuarterNote);
			return currentTrackTempoSectionStart + std::chrono::microseconds(aTick * microsecondsPerTick);
		};

	decoder.OnNewTrack.Connect(this, [&]()
		{
			currentTrackLoadData = ChartTrackLoadData();
			currentTrack.reset();
			currentTrackTempoSectionStart = std::chrono::microseconds(0);
			currentTrackTempo = static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::minutes(1)).count()) * 120; // 120 Beats Per Minute (MIDI default)
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
			if (currentTrack)
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
			if (currentTrack)
				currentTrackLoadData.AddLyric(ticksToTime(aTick), aText);
		}
	);

	decoder.OnTrackEnd.Connect(this, [&](std::uint32_t aTick)
		{
			myChartDuration = std::max(myChartDuration, ticksToTime(aTick));
			if (!currentTrack)
				return;

			if (currentTrack->Load(currentTrackLoadData) && !myTracks.contains(currentTrack->GetType()))
				myTracks[currentTrack->GetType()] = std::move(currentTrack);

			currentTrack.reset();
		}
	);

	decoder.OnSetTempo.Connect(this, [&](std::uint32_t aTick, std::uint32_t aTempo)
		{
			currentTrackTempoSectionStart = ticksToTime(aTick);
			currentTrackTempo = aTempo;
			myTempos.emplace_back(currentTrackTempoSectionStart, aTempo);
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

//std::chrono::microseconds ChartData::TicksToTime(std::uint32_t aTick) const
//{
//	std::chrono::microseconds totalTimeAtTick(0);
//
//	for (auto tempoSection = myTempos.begin(); tempoSection != myTempos.end(); ++tempoSection)
//	{
//		if (tempoSection->first > aTick)
//			break;
//
//		auto nextTempoSection = tempoSection + 1;
//		const std::chrono::microseconds microsecondsPerTick(tempoSection->second / myTicksPerQuarterNote);
//
//		const bool isContainingSection = (nextTempoSection == myTempos.end() || (tempoSection->first <= aTick && aTick < nextTempoSection->first));
//
//		std::int64_t sectionTickCount = ((isContainingSection ? aTick : nextTempoSection->first) - tempoSection->first);
//		totalTimeAtTick += std::chrono::microseconds(sectionTickCount * microsecondsPerTick);
//	}
//
//	return totalTimeAtTick;
//}
//
//std::uint32_t ChartData::TimeToTicks(std::chrono::microseconds aTime) const
//{
//	std::chrono::microseconds totalTimeAtTick(0);
//
//	for (auto tempoSection = myTempos.begin(); tempoSection != myTempos.end(); ++tempoSection)
//	{
//		std::chrono::microseconds sectionDuration(0);
//
//		const std::chrono::microseconds microsecondsPerTick(tempoSection->second / myTicksPerQuarterNote);
//
//		auto nextTempoSection = tempoSection + 1;
//		bool isCurrentSection = nextTempoSection == myTempos.end();
//		if (!isCurrentSection)
//		{
//			const std::uint32_t sectionTickCount = nextTempoSection->first - tempoSection->first;
//			sectionDuration = std::chrono::microseconds(sectionTickCount * microsecondsPerTick);
//			isCurrentSection = (totalTimeAtTick <= aTime) && aTime < (totalTimeAtTick + sectionDuration);
//		}
//
//		if (isCurrentSection)
//		{
//			const std::chrono::microseconds timeInSection = (aTime - totalTimeAtTick);
//			const std::uint32_t microsecondsInSection = static_cast<std::uint32_t>(timeInSection.count());
//			return tempoSection->first + (microsecondsInSection / myTicksPerQuarterNote);
//		}
//		else
//		{
//			totalTimeAtTick += sectionDuration;
//		}
//	}
//
//	return 0;
//}

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
