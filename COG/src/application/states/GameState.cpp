#include "GameState.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
GameState::GameState(GameState* previousState)	
	:	m_previousState(previousState),
		m_isActive(true),
		m_isAlive(true)
{

}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
GameState::~GameState()
{

}


/*******************************************************************************************************************
	Accessor Methods
*******************************************************************************************************************/
bool& GameState::IsActive() { return m_isActive; }
bool& GameState::IsAlive()	{ return m_isAlive; }