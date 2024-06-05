// Filter "Chart/Playback"
#pragma once

#include <chrono>

class ChartData;

class ChartPlayer
{
public:
	enum class State { Playing, Paused, Seeking, Stopped };

public:
	std::chrono::microseconds GetPlayhead() const { return myPlayhead; }

	State GetState() const;

	void Pause();

	void Play();

	void Seek(std::chrono::microseconds aPlayTime);

	void SetChartData(const ChartData& aData);

	void Stop();

	void Update();

private:
	enum class InternalState
	{
		Playing, Paused, SeekingPlaying, SeekingPaused, Stopped
	};

	const ChartData* myChartData = nullptr;

	InternalState myState = InternalState::Stopped;
	std::chrono::high_resolution_clock::time_point myStartTime;
	std::chrono::high_resolution_clock::time_point myLastUpdateTime;

	std::chrono::microseconds myPlayhead;
};
