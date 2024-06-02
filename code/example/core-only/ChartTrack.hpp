// Filter "Chart"
#pragma once

#include "ChartEnums.hpp"

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ChartTrack
{
public:
	static std::unique_ptr<ChartTrack> CreateTrackByName(const std::string& aName);

public:
	virtual ~ChartTrack() = default;

	virtual void AddNote(std::uint8_t aNote, std::uint32_t aNoteStart, std::uint32_t aNoteEnd) = 0;

	ChartTrackType GetType() const { return myType; }

private:
	ChartTrackType myType;
};

class ChartGuitarTrack : public ChartTrack
{
public:
	enum class Note
	{
		Open,
		Green,
		Red,
		Yellow,
		Blue,
		Orange,

		ForceHOPO,
		ForceStrum
	};

	struct NoteRange
	{
		Note Note = Note::Open;
		std::uint32_t Start = 0;
		std::uint32_t End = 0;
	};

	enum class Marker
	{
		Solo,
		TapNote,
		P1VersusPhase,
		P2VersusPhase,

		StarPower,

		BigRockEnding1,
		BigRockEnding2,
		BigRockEnding3,
		BigRockEnding4,
		BigRockEnding5,

		TremoloLane,
		TrillLane
	};

	struct MarkerRange
	{
		Marker Marker = Marker::Solo;
		std::uint32_t Start = 0;
		std::uint32_t End = 0;
	};

public:
	void AddNote(std::uint8_t aNote, std::uint32_t aNoteStart, std::uint32_t aNoteEnd) override;

private:
	std::map<ChartTrackDifficulty, std::vector<NoteRange>> myNotes;
	std::vector<MarkerRange> myMarkers;
};
