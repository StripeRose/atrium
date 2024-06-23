// Filter "Chart/Playback"
#include "stdafx.hpp"
#include "ChartPlayback.hpp"

#include "ChartData.hpp"
#include "Common_Diagnostics.hpp"

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
	RoseGold::Debug::Log("Chart pause.");
	myState = InternalState::Paused;
}

void ChartPlayer::Play()
{
	if (myChartData == nullptr)
		return;

	RoseGold::Debug::Log("Chart play.");
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

void ChartPlayer::RemoveController(ChartController& aController)
{
	const auto it = std::find_if(
		myControllers.begin(),
		myControllers.end(),
		[&aController](const std::unique_ptr<ChartController>& listController)
		{ return listController.get() == &aController; }
	);

	myControllers.erase(it);
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
	for (const std::unique_ptr<ChartController>& controller : myControllers)
		controller->OnChartChange(aData);
}

void ChartPlayer::Stop()
{
	RoseGold::Debug::Log("Chart stop.");
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
		for (const std::unique_ptr<ChartController>& controller : myControllers)
			controller->OnPlayheadStep(lastUpdatePoint, thisUpdatePoint);
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
