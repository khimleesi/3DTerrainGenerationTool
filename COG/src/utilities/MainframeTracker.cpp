#include "MainframeTracker.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
MainframeTracker::MainframeTracker()
	:	m_mainframePollingEnabled(true),
		m_queryHandle(nullptr),
		m_counterHandle(nullptr),
		m_mainframeUsage(0),
		m_lastSampleTime(0)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
MainframeTracker::~MainframeTracker()
{

}


/*******************************************************************************************************************
	Function that starts the mainframe tracker, which allows us to query the usage of CPU
*******************************************************************************************************************/
void MainframeTracker::Start()
{
	PDH_STATUS status = 0;

	//--- Create a query object to poll CPU usage
	status = PdhOpenQuery(nullptr, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS) { m_mainframePollingEnabled = false; }

	//--- Set query object to poll all CPU's in the system
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS) { m_mainframePollingEnabled = false; }
}


/*******************************************************************************************************************
	Function that updates the mainframe tracker every frame
*******************************************************************************************************************/
void MainframeTracker::Update()
{
	int seconds = 1000;

	PDH_FMT_COUNTERVALUE value = {};

	if (m_mainframePollingEnabled)
	{
		//--- If it has been 1 second then update the current CPU usage and reset the 1 second timer again
		if ((m_lastSampleTime + seconds) < GetTickCount()) {

			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_mainframeUsage = value.longValue;
		}
	}
}


/*******************************************************************************************************************
	Function that stops the mainframe tracker, which must be done at the end of the game
*******************************************************************************************************************/
void MainframeTracker::Stop()
{
	if (m_mainframePollingEnabled) { PdhCloseQuery(m_queryHandle); m_queryHandle = nullptr; }
}


/*******************************************************************************************************************
	Function that returns the cpu usage %
*******************************************************************************************************************/
int MainframeTracker::GetMainframePercentage()
{
	int usage = 0;

	//--- If we can read the CPU % from the operating system then return the current usage
	if (m_mainframePollingEnabled) { usage = (int)m_mainframeUsage; }

	return usage;
}