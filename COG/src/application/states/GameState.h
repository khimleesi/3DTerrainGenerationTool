#pragma once
/*******************************************************************************************************************
	GameState.h, GameState.cpp
	Created by Kim Kane
	Last updated: 06/01/2018

	Base class for all our game states.

*******************************************************************************************************************/


class GameState {

public:
	GameState(GameState* previousState);
	virtual ~GameState() = 0;

public:
	virtual bool Update()	= 0;
	virtual bool Render()	= 0;

public:
	bool& IsActive();
	bool& IsAlive();

protected:
	template <typename T, typename C> void AddToScene(C& container, T* object);
	template <typename C> void RemoveFromScene(C& container);
	template <typename C> void ReserveMemory(C& container, unsigned int size);

protected:
	GameState*	m_previousState;

private:
	bool		m_isActive;
	bool		m_isAlive;
};


/*******************************************************************************************************************
	Adds an object to container
*******************************************************************************************************************/
template <typename T, typename C>
void GameState::AddToScene(C& container, T* object)
{
	container.emplace_back(object);
}


/*******************************************************************************************************************
	Reserves memory of container
*******************************************************************************************************************/
template <typename C>
void GameState::ReserveMemory(C& container, unsigned int size)
{
	container.reserve(size);
}


/*******************************************************************************************************************
	Removes and deletes all heap allocated objects from passed in container
*******************************************************************************************************************/
template <typename C>
void GameState::RemoveFromScene(C& container)
{
	for (auto& object : container)	{ delete object; object = nullptr; }
	container.clear();
}