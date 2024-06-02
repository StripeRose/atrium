// Filter "Chart/Playback"
#include "stdafx.hpp"
#include "ChartPlayback.hpp"

void ChartPlayer::Play(std::int32_t aSongOffset)
{
	myStartPoint = std::chrono::high_resolution_clock::now();
	mySongTicksOffset = aSongOffset;
	myIsPlaying = true;
}

void ChartPlayer::SetChartData(const ChartData& aData)
{
	myChartData = &aData;
}

void ChartPlayer::Stop()
{
	myIsPlaying = false;
}
