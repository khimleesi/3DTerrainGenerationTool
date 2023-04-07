#pragma once
/*******************************************************************************************************************
	Timer.h, Timer.cpp
	Created by Kim Kane
	Last updated: 14/04/2018
	Class finalized: 14/04/2018

	Creates a simple timer used to track FPS, frame count and update count.

	[Features]
	Supports Windows and chrono timers.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <Windows.h>
#include <chrono>

class Timer {

private:
	typedef std::chrono::high_resolution_clock HighResolutionClock;
	typedef std::chrono::duration<float, std::milli> Milliseconds;

	struct ChronoTimer
	{
		std::chrono::time_point<HighResolutionClock> start;
	};

	struct WindowsTimer
	{
		LARGE_INTEGER start;
		double frequency;
	};

	struct FrameTimer
	{
		float previousSeconds;
		float previousTicks;
		unsigned int frames;
		unsigned int updates;
	};

public:
	Timer(bool chrono);
	~Timer();

public:
	bool	Start();
	void	Reset();
	float	ElapsedSeconds();
	float	ElapsedMilliseconds();
	
public:
	FrameTimer*		GetThisFrame();
	unsigned int	GetFramesPerSecond() const;
	unsigned int	GetUpdatesPerSecond() const;
	float			GetCurrentFrameTime() const;

public:
	void SetThisFrame(float previousSeconds, float previousTicks, unsigned int updates, unsigned int frames);
	void SetFramesPerSecond(unsigned int frames);
	void SetUpdatesPerSecond(unsigned int updates);
	void SetFrameTime(float frameTime);

public:
	static const float GetDefaultFrameTime();
	static const float GetMaxDeltaTime();
	static const float GetMaxSeconds();

private:
	bool			m_useChrono;
	ChronoTimer		m_chronoTimer;
	WindowsTimer	m_windowsTimer;
	FrameTimer		m_frameTimer;

private:
	float			m_frameTime;
	unsigned int	m_framesPerSecond;
	unsigned int	m_updatesPerSecond;

private:
	static const float s_msPerSecond;
	static const float s_defaultFps;
	static const float s_maxDeltaTime;
	static const float s_maxSeconds;
	static const float s_defaultFrameTime;
};