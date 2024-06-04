// Filter "Chart"
#include "stdafx.hpp"
#include "ChartTrack.hpp"

#include "MidiDecoder.hpp"

#include "Common_Diagnostics.hpp"

std::unique_ptr<ChartTrack> ChartTrack::CreateTrackByName(const std::string& aName)
{
	std::unique_ptr<ChartTrack> track;
	if (aName == "GUITAR")
	{
		track.reset(new ChartGuitarTrack());
		track->myType = ChartTrackType::LeadGuitar;
	}
	else if (aName == "RHYTHM")
	{
		track.reset(new ChartGuitarTrack());
		track->myType = ChartTrackType::RhythmGuitar;
	}
	else if (aName == "BASS")
	{
		track.reset(new ChartGuitarTrack());
		track->myType = ChartTrackType::BassGuitar;
	}
	else
	{
		RoseGold::Debug::LogError("Unknown track type: %s", aName.c_str());
	}

	return track;
}

void ChartGuitarTrack::AddNote(std::uint8_t aNote, std::chrono::microseconds aNoteStart, std::chrono::microseconds aNoteEnd)
{
	if (aNote < 103)
	{
		std::uint8_t lane = 0;
		std::uint8_t difficulty = 0;
		if (aNote >= 59 && aNote <= 66)
		{
			difficulty = 0;
			lane = aNote - 59;
		}
		else if (aNote >= 71 && aNote <= 78)
		{
			difficulty = 1;
			lane = aNote - 71;
		}
		else if (aNote >= 83 && aNote <= 90)
		{
			difficulty = 2;
			lane = aNote - 83;
		}
		else if (aNote >= 95 && aNote <= 102)
		{
			difficulty = 3;
			lane = aNote - 95;
		}
		else
		{
			RoseGold::Debug::LogWarning("Unknown note %s (%u)", MidiDecoder::NoteNumberToString(aNote).c_str(), aNote);
		}

		if (mySysEx_TapNote.test(difficulty))
		{
			NoteRange& hopo = myNotes[ChartTrackDifficulty(difficulty)].emplace_back();
			hopo.Note = Note::ForceHOPO;
			hopo.Start = aNoteStart;
			hopo.End = aNoteEnd;
		}

		NoteRange& note = myNotes[ChartTrackDifficulty(difficulty)].emplace_back();
		note.Note = Note(lane);
		note.Start = aNoteStart;
		note.End = aNoteEnd;

		if (mySysEx_OpenNote.test(difficulty))
			note.Note = Note::Open;
	}
	else
	{
		MarkerRange& marker = myMarkers.emplace_back();
		marker.Start = aNoteStart;
		marker.End = aNoteEnd;

		switch (aNote)
		{
		case 103: marker.Marker = Marker::Solo; break;
		case 104: marker.Marker = Marker::TapNote; break;
		case 105: marker.Marker = Marker::P1VersusPhase; break;
		case 106: marker.Marker = Marker::P2VersusPhase; break;

		case 116: marker.Marker = Marker::StarPower; break;

		case 120: marker.Marker = Marker::BigRockEnding5; break;
		case 121: marker.Marker = Marker::BigRockEnding4; break;
		case 122: marker.Marker = Marker::BigRockEnding3; break;
		case 123: marker.Marker = Marker::BigRockEnding2; break;
		case 124: marker.Marker = Marker::BigRockEnding1; break;

		case 126: marker.Marker = Marker::TremoloLane; break;
		case 127: marker.Marker = Marker::TrillLane; break;

		default:
			RoseGold::Debug::LogWarning("Unknown note %s (%u)", MidiDecoder::NoteNumberToString(aNote).c_str(), aNote);
			break;
		}
	}
}

void ChartGuitarTrack::AddSysEx(std::chrono::microseconds aTime, const std::span<std::uint8_t>& someData)
{
	if (AddPhaseShift(aTime, someData))
		return;

	std::string dataInHex;
	dataInHex.reserve(someData.size() * 3);
	static const char* digits = "0123456789ABCDEF";
	for (std::uint8_t d : someData)
	{
		dataInHex += digits[(d >> 4) & 0xF];
		dataInHex += digits[d & 0xF];
		dataInHex += ' ';
	}

	RoseGold::Debug::Log("Unknown SysEx event - % s", dataInHex.c_str());
}

bool ChartGuitarTrack::AddPhaseShift(std::chrono::microseconds, const std::span<std::uint8_t>& someData)
{
	if (someData.size() != 7)
		return false;

	if (someData[0] != 'P' || someData[1] != 'S')
		return false;

	static constexpr std::uint8_t Easy = 0;
	static constexpr std::uint8_t Medium = 1;
	static constexpr std::uint8_t Hard = 2;
	static constexpr std::uint8_t Expert = 3;
	static constexpr std::uint8_t All = 0xFF;

	static constexpr std::uint8_t OpenNote = 0x10;
	static constexpr std::uint8_t TapNote = 0x40;

	const std::uint8_t difficulty = someData[4];
	const std::uint8_t type = someData[5];
	const std::uint8_t value = someData[6];

	PerDifficultyFlag* flag = nullptr;
	if (type == 0x01)
		flag = &mySysEx_OpenNote;
	else if (type == 0x04)
		flag = &mySysEx_TapNote;
	else
		return false;

	if (difficulty == All)
	{
		for (std::uint8_t i = 0; i < static_cast<std::uint8_t>(ChartTrackDifficulty::Count); ++i)
			flag->set(i, value != 0);
	}
	else
	{
		flag->set(difficulty, value != 0);
	}

	return true;
}
