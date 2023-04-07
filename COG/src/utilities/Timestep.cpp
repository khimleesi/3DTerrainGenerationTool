#include "Timestep.h"
#include "Timer.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Timestep::Timestep(float initialTime)
	: m_timestep(0.0f), m_previousTicks(initialTime)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Timestep::~Timestep()
{

}


/*******************************************************************************************************************
	A function that updates the timestep and sets it to the (current time passed in - the last time registered)
*******************************************************************************************************************/
void Timestep::Update(float currentTicks)
{
	m_timestep		= (currentTicks - m_previousTicks);
	m_previousTicks = currentTicks;
}


/*******************************************************************************************************************
	A function that calculates the delta time
*******************************************************************************************************************/
float Timestep::CalculateDeltaTime() const { return (m_timestep / Timer::GetDefaultFrameTime()); }


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
float Timestep::GetMilliseconds() const { return m_timestep; }
float Timestep::GetSeconds() const		{ return m_timestep * s_secondPerMs; }
float Timestep::GetDeltaTime() const	{ return min(CalculateDeltaTime(), Timer::GetMaxDeltaTime()); }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Timestep::SetInitialTime(float initialTime) { m_previousTicks = initialTime; }

/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const float Timestep::s_secondPerMs		= 0.001f;