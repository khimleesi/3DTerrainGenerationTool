#include "Timer.h"
#include "Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Timer::Timer(bool chrono)
	:	m_useChrono(chrono),
		m_frameTime(0.0f),
		m_framesPerSecond(0),
		m_updatesPerSecond(0)
{
	
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Timer::~Timer()
{

}


/*******************************************************************************************************************
	A function that starts the timer (chrono/windows)
*******************************************************************************************************************/
bool Timer::Start()
{
	//--- If using a windows timer, do the following
	if (!m_useChrono)
	{
		LARGE_INTEGER frequency = {};
		
		if (!QueryPerformanceFrequency(&frequency))
		{
			COG_LOG("[TIMER] Could not acquire frequency for windows timer", COG_LOG_EMPTY, LOG_ERROR);
			return false;
		}

		//--- Get the minimum frequency
		m_windowsTimer.frequency = 1.0 / frequency.QuadPart;
	}

	//--- Get the current time of the timer
	Reset();

	return true;
}


/*******************************************************************************************************************
	A function resets the timer and registers the current time
*******************************************************************************************************************/
void Timer::Reset()
{
	//--- If using a windows timer, do the following
	if (!m_useChrono) { 
		
		if (!QueryPerformanceCounter(&m_windowsTimer.start))
		{
			COG_LOG("[TIMER] Could not initialize start time for windows timer", COG_LOG_EMPTY, LOG_ERROR);
		}
	}

	//--- Otherwise, use chrono to get the current time
	else { m_chronoTimer.start = HighResolutionClock::now(); }
}


/*******************************************************************************************************************
	A function that returns the elapsed seconds passed since the timer began
*******************************************************************************************************************/
float Timer::ElapsedSeconds()
{
	//--- If using a windows timer, do the following
	if (!m_useChrono)
	{
		LARGE_INTEGER current = {};
		
		if (!QueryPerformanceCounter(&current))
		{
			COG_LOG("[TIMER] Could not get current time for windows timer", COG_LOG_EMPTY, LOG_ERROR);
			return 0.0f;
		}

		//--- Get the cycles (current time - previous time)
		UINT64 cycles = (current.QuadPart - m_windowsTimer.start.QuadPart);
	
		//--- Return this time in seconds
		return (float)(cycles * m_windowsTimer.frequency);
	}

	//--- If using chrono return the (current time - previous time) in seconds
	return std::chrono::duration_cast<Milliseconds>(HighResolutionClock::now() - m_chronoTimer.start).count() / s_msPerSecond;
}


/*******************************************************************************************************************
	A function that returns the elapsed milliseconds passed since the timer began
*******************************************************************************************************************/
float Timer::ElapsedMilliseconds()
{
	return ElapsedSeconds() * s_msPerSecond;
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
float Timer::GetCurrentFrameTime() const			{ return m_frameTime; }
unsigned int Timer::GetFramesPerSecond() const		{ return m_framesPerSecond; }
unsigned int Timer::GetUpdatesPerSecond() const		{ return m_updatesPerSecond; }
Timer::FrameTimer* Timer::GetThisFrame()			{ return &m_frameTimer; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Timer::SetFrameTime(float frameTime)				{ m_frameTime = frameTime; }
void Timer::SetFramesPerSecond(unsigned int frames)		{ m_framesPerSecond = frames; }
void Timer::SetUpdatesPerSecond(unsigned int updates)	{ m_updatesPerSecond = updates; }

void Timer::SetThisFrame(float previousSeconds, float previousTicks, unsigned int updates, unsigned int frames)
{
	m_frameTimer.previousSeconds	= previousSeconds;
	m_frameTimer.previousTicks		= previousTicks;
	m_frameTimer.updates			= updates;
	m_frameTimer.frames				= frames;
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const float Timer::s_msPerSecond		= 1000.0f;
const float Timer::s_defaultFps			= 60.0f;
const float Timer::s_maxDeltaTime		= 1.0f;
const float Timer::s_defaultFrameTime	= (s_msPerSecond / s_defaultFps);
const float Timer::s_maxSeconds			= 1.0f;

const float Timer::GetDefaultFrameTime()	{ return s_defaultFrameTime; }
const float Timer::GetMaxDeltaTime()		{ return s_maxDeltaTime; }
const float Timer::GetMaxSeconds()			{ return s_maxSeconds; }