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

void ChartData::Load(const std::filesystem::path& aDirectory)
{
	const std::filesystem::path songIni = aDirectory / "song.ini";
	const std::filesystem::path notesMid = aDirectory / "notes.mid";
	const std::filesystem::path notesMidi = aDirectory / "notes.midi";

	if (!std::filesystem::exists(songIni))
		throw std::runtime_error("Song.ini missing.");

	myInfo.Load(aDirectory / "song.ini");

	if (std::filesystem::exists(notesMid))
		LoadMidi(notesMid);
	else if (std::filesystem::exists(notesMidi))
		LoadMidi(notesMidi);
	else
		throw std::runtime_error("Notes file missing.");
}

void ChartData::LoadMidi(const std::filesystem::path& aMidi)
{
	MidiDecoder decoder;

	MidiDecoder::FormatType formatType = MidiDecoder::FormatType::SingleTrack;
	std::uint16_t ticksPerQuarterNote = 0;

	ChartTrack* currentTrack = nullptr;

	std::map<std::uint8_t, std::uint32_t> myPartialNotes;

	using namespace RoseCommon;

	decoder.OnNewTrack.Connect(this, [&]()
		{
			Debug::Log("--- [ TRACK ] -------------------------");
			currentTrack = nullptr;
			myPartialNotes.clear();
		});

	decoder.OnTrackName.Connect(this, [&](std::uint32_t, const std::string& aText) {
		Debug::Log("NAME: %s", aText.c_str());
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
		});

	decoder.OnNote.Connect(this, [&](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aNote, std::uint8_t aVelocity)
		{
			Debug::Log("%08u Ch%u: Note %s (%u) = %u", aTick, aChannel, MidiDecoder::NoteNumberToString(aNote).c_str(), aNote, aVelocity);
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
		});

	decoder.OnNotePressure.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aNote, std::uint8_t aPressure) {
		Debug::Log("%08u Ch%u: Note %s (%u) Pressure %u", aTick, aChannel, MidiDecoder::NoteNumberToString(aNote).c_str(), aNote, aPressure);
		});

	decoder.OnSoundOff.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: Sound off.", aTick, aChannel);
		});
	decoder.OnResetAllControllers.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aValue) {
		Debug::Log("%08u Ch%u: Reset controllers. (%u)", aTick, aChannel, aValue);
		});
	decoder.OnLocalControl.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: Local control.", aTick, aChannel);
		});
	decoder.OnAllNotesOff.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: All notes off.", aTick, aChannel);
		});
	decoder.OnOmniOff.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: Omni off.", aTick, aChannel);
		});
	decoder.OnOmniOn.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: Omni on.", aTick, aChannel);
		});
	decoder.OnMonoOn.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aValue) {
		Debug::Log("%08u Ch%u: Mono on, poly off. (%u)", aTick, aChannel, aValue);
		});
	decoder.OnPolyOn.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel) {
		Debug::Log("%08u Ch%u: Poly on, mono off.", aTick, aChannel);
		});

	decoder.OnProgramChange.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aProgramNum) {
		Debug::Log("%08u Ch%u: Program change: %u", aTick, aChannel, aProgramNum);
		});
	decoder.OnChannelPressure.Connect(this, [](std::uint32_t aTick, std::uint8_t aChannel, std::uint8_t aPressure) {
		Debug::Log("%08u Ch%u: Channel pressure: %u", aTick, aChannel, aPressure);
		});

	decoder.OnSysEx.Connect(this, [](std::uint32_t aTick, const std::span<std::uint8_t>& someData)
		{
			std::string dataInHex;
			dataInHex.reserve(someData.size() * 3);
			static const char* digits = "0123456789ABCDEF";
			for (std::uint8_t d : someData)
			{
				dataInHex += digits[d & 0xF];
				dataInHex += digits[(d >> 4) & 0xF];
				dataInHex += ' ';
			}

			Debug::Log("%08u: SysEx - %s", aTick, dataInHex.c_str());
		});

	decoder.OnSequenceNumber.Connect(this, [](std::uint32_t aTick, std::uint16_t aNum) {
		Debug::Log("%08u: Sequence num %i", aTick, aNum);
		});
	decoder.OnText.Connect(this, [&](std::uint32_t aTick, const std::string& aText) {
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
		});
	// Todo: Lyric events for Vocal tracks.
	decoder.OnLyric.Connect(this, [](std::uint32_t aTick, const std::string& aText) {
		Debug::Log("%08u: Lyric: \"%s\"", aTick, aText.c_str());
		});
	decoder.OnMarker.Connect(this, [](std::uint32_t aTick, const std::string& aText) {
		Debug::Log("%08u: Marker: \"%s\"", aTick, aText.c_str());
		});
	decoder.OnCuePoint.Connect(this, [](std::uint32_t aTick, const std::string& aText) {
		Debug::Log("%08u: Cue: \"%s\"", aTick, aText.c_str());
		});

	decoder.OnTrackEnd.Connect(this, [&](std::uint32_t aTick) {
		Debug::Log("%08u: End of track", aTick);
		currentTrack = nullptr;
		});
	decoder.OnSetTempo.Connect(this, [&](std::uint32_t aTick, std::uint32_t aTempo) {
		Debug::Log("%08u: Set tempo %u", aTick, aTempo);
		myTempos.emplace_back(aTick, aTempo);
		});

	decoder.OnTimeSignature.Connect(this, [&](std::uint32_t aTick, std::uint8_t aNumerator, std::uint8_t aDenominator, std::uint8_t aClock, std::uint8_t aBase) {
		Debug::Log("%08u: Time signature %u/%u;%u;%u", aTick, aNumerator, aDenominator, aClock, aBase);
		myTimeSignatures.emplace_back(aTick, TimeSignature{ aNumerator, aDenominator, aClock, aBase });
		});
	decoder.OnKeySignature.Connect(this, [](std::uint32_t aTick, std::uint8_t aSharpFlat, std::uint8_t aMajorMinor) {
		Debug::Log("%08u: Key signature SharpFlat %u, MajorMinor: %u", aTick, aSharpFlat, aMajorMinor);
		});

	decoder.ProcessFile(aMidi, formatType, ticksPerQuarterNote);
}
