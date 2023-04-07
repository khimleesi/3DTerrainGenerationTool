#pragma once
/*******************************************************************************************************************
	MainframeTracker.h, MainframeTracker.cpp
	Created by Kim Kane
	Last updated: 14/04/2018
	Class finalized: 14/04/2018

	Creates a simple mainframe tracker, used to track % of the CPU usage.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "winmm.lib")

#include <pdh.h>
#include <Windows.h>
#include <mmsystem.h>

class MainframeTracker {

public:
	MainframeTracker();
	~MainframeTracker();

public:
	void Start();
	void Update();
	void Stop();

public:
	int GetMainframePercentage();

private:
	bool			m_mainframePollingEnabled;
	HQUERY			m_queryHandle;
	HCOUNTER		m_counterHandle;
	long			m_mainframeUsage;
	unsigned long	m_lastSampleTime;
};