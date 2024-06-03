// Filter "Chart/Playback"
#pragma once

#include <chrono>

class ChartData;

class ChartPlayer
{
public:
	std::uint32_t GetTick() const { return 0; }

	bool IsPlaying() const { return myIsPlaying; }

	void Play(std::int32_t aSongOffset = 0);

	void SetChartData(const ChartData& aData);

	void Stop();

private:
	const ChartData* myChartData;

	bool myIsPlaying = false;
	std::chrono::high_resolution_clock::time_point myStartPoint;
	std::int32_t mySongTicksOffset = 0;
};
