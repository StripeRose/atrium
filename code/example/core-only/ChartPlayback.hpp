// Filter "Chart/Playback"
#pragma once

#include <chrono>

class ChartData;

class ChartPlayer
{
public:
	std::chrono::microseconds GetPlayhead() const { return myPlayhead; }

	bool IsPlaying() const { return myIsPlaying; }

	void Play(std::chrono::microseconds aPlayTime = std::chrono::microseconds(0));

	void SetChartData(const ChartData& aData);

	void Stop();

	void Update();

private:
	const ChartData* myChartData = nullptr;

	bool myIsPlaying = false;
	std::chrono::high_resolution_clock::time_point myStartTime;
	std::chrono::high_resolution_clock::time_point myLastUpdateTime;

	std::chrono::microseconds myPlayhead;
};
