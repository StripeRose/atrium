// Filter "Chart"
#include "stdafx.hpp"
#include "ChartTrack.hpp"

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

void ChartGuitarTrack::AddNote(std::uint8_t aNote, std::uint32_t aNoteStart, std::uint32_t aNoteEnd)
{
	if (aNote < 103)
	{
		std::vector<NoteRange>* difficulty;
		std::uint8_t offset = 0;
		if (aNote >= 59 && aNote <= 66)
		{
			difficulty = &myNotes[ChartTrackDifficulty::Easy];
			offset = 59;
		}
		else if (aNote >= 71 && aNote <= 78)
		{
			difficulty = &myNotes[ChartTrackDifficulty::Medium];
			offset = 71;
		}
		else if (aNote >= 83 && aNote <= 90)
		{
			difficulty = &myNotes[ChartTrackDifficulty::Hard];
			offset = 83;
		}
		else if (aNote >= 95 && aNote <= 102)
		{
			difficulty = &myNotes[ChartTrackDifficulty::Expert];
			offset = 95;
		}
		else
		{
			return;
		}

		const std::uint8_t lane = aNote - offset;
		NoteRange& note = difficulty->emplace_back();
		note.Note = Note(lane);
		note.Start = aNoteStart;
		note.End = aNoteEnd;
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
		}
	}
}
