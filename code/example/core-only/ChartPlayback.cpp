// Filter "Chart/Playback"
#include "stdafx.hpp"
#include "ChartPlayback.hpp"

#include "ChartData.hpp"

void ChartPlayer::Play(std::chrono::microseconds aPlayTime)
{
	myStartTime = myLastUpdateTime = std::chrono::high_resolution_clock::now();
	myPlayhead = aPlayTime;
	myIsPlaying = true;
}

void ChartPlayer::SetChartData(const ChartData& aData)
{
	myChartData = &aData;
}

void ChartPlayer::Stop()
{
	myIsPlaying = false;
	myPlayhead = std::chrono::microseconds(0);
}

void ChartPlayer::Update()
{
	if (!myIsPlaying)
		return;

	const auto newUpdateTime = std::chrono::high_resolution_clock::now();
	const auto lastUpdatePoint = std::chrono::duration_cast<std::chrono::microseconds>(myLastUpdateTime - myStartTime);
	const auto thisUpdatePoint = std::chrono::duration_cast<std::chrono::microseconds>(newUpdateTime - myStartTime);

	// Todo: Process (lastUpdatePoint <= X < thisUpdatePoint)

	myLastUpdateTime = newUpdateTime;
	myPlayhead = thisUpdatePoint;
}
