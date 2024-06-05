// Filter "Chart/Playback"
#include "stdafx.hpp"
#include "ChartPlayback.hpp"

#include "ChartData.hpp"

ChartPlayer::State ChartPlayer::GetState() const
{
	switch (myState)
	{
	case InternalState::Playing:
		return State::Playing;
	case InternalState::Paused:
		return State::Paused;
	case InternalState::SeekingPaused:
	case InternalState::SeekingPlaying:
		return State::Seeking;
	case InternalState::Stopped:
		return State::Stopped;
	}

	return State::Stopped;
}

void ChartPlayer::Pause()
{
	myState = InternalState::Paused;
}

void ChartPlayer::Play()
{
	switch (myState)
	{
	case InternalState::Playing:
		return;
	case InternalState::Paused:
		myState = InternalState::Playing;
		return;
	case InternalState::SeekingPaused:
	case InternalState::SeekingPlaying:
		return;
	case InternalState::Stopped:
		myStartTime = std::chrono::high_resolution_clock::now();
		myLastUpdateTime = myStartTime;
		myState = InternalState::Playing;
		return;
	}
}

void ChartPlayer::Seek(std::chrono::microseconds aPlayTime)
{
	switch (myState)
	{
	case InternalState::Playing:
		myState = InternalState::SeekingPlaying;
		break;
	case InternalState::Paused:
		myState = InternalState::SeekingPaused;
		break;
	default:
		return;
	}

	myPlayhead = aPlayTime;
}

void ChartPlayer::SetChartData(const ChartData& aData)
{
	myChartData = &aData;
}

void ChartPlayer::Stop()
{
	myState = InternalState::Stopped;
	myPlayhead = std::chrono::microseconds(0);
}

void ChartPlayer::Update()
{
	const auto newUpdateTime = std::chrono::high_resolution_clock::now();
	const auto lastUpdatePoint = std::chrono::duration_cast<std::chrono::microseconds>(myLastUpdateTime - myStartTime);
	const auto thisUpdatePoint = std::chrono::duration_cast<std::chrono::microseconds>(newUpdateTime - myStartTime);

	myLastUpdateTime = newUpdateTime;

	switch (myState)
	{
	case InternalState::Playing:
		// Todo: Process (lastUpdatePoint <= X < thisUpdatePoint)
		myPlayhead = thisUpdatePoint;
		break;
	case InternalState::Paused:
		myStartTime += (thisUpdatePoint - lastUpdatePoint);
		break;
	case InternalState::SeekingPlaying:
	case InternalState::SeekingPaused:
		myStartTime = (newUpdateTime - myPlayhead);
		myState = (myState == InternalState::SeekingPlaying ? InternalState::Playing : InternalState::Paused);
		break;
	case InternalState::Stopped:
		break;
	}
}
