// Filter "Chart/Playback"

#pragma once

class ChartData;

class ChartPlayback
{
public:
	void SetChartData(const ChartData& aData);

private:
	const ChartData* myChartData;
};
