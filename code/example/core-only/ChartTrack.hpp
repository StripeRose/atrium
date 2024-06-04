// Filter "Chart"
#pragma once

#include "ChartEnums.hpp"

#include <array>
#include <bitset>
#include <chrono>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <vector>

class ChartTrack
{
public:
	static std::unique_ptr<ChartTrack> CreateTrackByName(const std::string& aName);

public:
	virtual ~ChartTrack() = default;

	virtual void AddNote(std::uint8_t aNote, std::chrono::microseconds aNoteStart, std::chrono::microseconds aNoteEnd) = 0;
	virtual void AddSysEx(std::chrono::microseconds aTime, const std::span<std::uint8_t>& someData) = 0;
	virtual void AddLyric(std::chrono::microseconds aTime, const std::string& aText) = 0;

	ChartTrackType GetType() const { return myType; }

private:
	ChartTrackType myType = ChartTrackType::LeadGuitar;
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
		std::chrono::microseconds Start = std::chrono::microseconds(0);
		std::chrono::microseconds End = std::chrono::microseconds(0);
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
		std::chrono::microseconds Start = std::chrono::microseconds(0);
		std::chrono::microseconds End = std::chrono::microseconds(0);
	};

public:
	void AddNote(std::uint8_t aNote, std::chrono::microseconds aNoteStart, std::chrono::microseconds aNoteEnd) override;
	void AddSysEx(std::chrono::microseconds aTime, const std::span<std::uint8_t>& someData) override;
	void AddLyric(std::chrono::microseconds, const std::string&) override { }

private:
	bool AddPhaseShift(std::chrono::microseconds aTime, const std::span<std::uint8_t>& someData);

	std::map<ChartTrackDifficulty, std::vector<NoteRange>> myNotes;
	std::vector<MarkerRange> myMarkers;

	using PerDifficultyFlag = std::bitset<static_cast<size_t>(ChartTrackDifficulty::Count)>;

	PerDifficultyFlag mySysEx_OpenNote;
	PerDifficultyFlag mySysEx_TapNote;
};
