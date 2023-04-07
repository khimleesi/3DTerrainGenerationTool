#pragma once
/*******************************************************************************************************************
	Timestep.h, Timestep.cpp
	Created by Kim Kane
	Last updated: 14/04/2018
	Class finalized: 14/04/2018

	Keeps track of our in-game delta time. To be used in conjunction with the Timer class.

	[Features]
	Nothing at present.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
class Timestep {

public:
	Timestep(float initialTime);
	~Timestep();

public:
	void Update(float currentTicks);
	void SetInitialTime(float initialTime);

public:
	float GetMilliseconds() const;
	float GetSeconds()		const;
	float GetDeltaTime()	const;

private:
	float CalculateDeltaTime()	const;

private:
	float m_timestep;
	float m_previousTicks;

private:
	static const float s_secondPerMs;
};