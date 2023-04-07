#pragma once

/*******************************************************************************************************************
	FiniteStateMachine.h, FiniteStateMachine.cpp
	Created by Kim Kane
	Last updated: 11/04/2018
	Class finalized: 11/04/2018

	A complete template class that creates a fully generic FSM for use anywhere.

	[Features]
	Fully templated and generic - allowing the owner of the FSM full access to the FSM to make custom adjustment's.
	Makes use of smart pointers (unique) to store the states, so no clean up is necessary and no worry of memory leaks.
	States are created in heap memory and will last until they go out of scope - no worry of crashes.
	Supports temporary and permanent states (e.g. pause or play)
	Has basic helper functions already implemented (more can be added by the implementer if needed)

	[Upcoming]
	Various different state machines', all generic and templated.
	E.g. the user may not always want to delete the states once loaded, but just re-use them if they
	have already been loaded. I'd like to try and implement all different FSM's so I can have complete
	control over which ones I would like to use, and where!
	UML state machines.

	[Side Notes]
	The FSM is just a template class that sets up a basic interface for a state machine, with a few additional
	helper functions. It is up to the implementer to further implement the correct functions in order to create 
	the kind of state machine they desire. The implementer is a friend of the FSM, allowing full access to all private variables.
	This allows the implementer complete control over the implementation of the FSM; allowing them to add additional functions
	to further aid their FSM implementation and tailor the FSM to their needs.

	The FSM is created as follows:

	FiniteStateMachine<GameManager, GameState> m_gameStates;

	Where GameManager is the implementer of the FSM, and GameState is the base class for the desired states.
	
	No other class apart from the implementer will have access to the FSM - the only public functions available
	are MakePermanentState(args...) and MakeTemporaryState(args...). The implementer can choose
	to make additional functions public if they wish.

*******************************************************************************************************************/
#include <deque>
#include <memory>

template <class Implementer, class BaseState>
class FiniteStateMachine {

	friend Implementer;

public:
	FiniteStateMachine();
	~FiniteStateMachine();

public:
	template<typename State, typename... Args>
	void MakePermanentState(Args... args);
	
	template<typename State, typename... Args>
	void MakeTemporaryState(Args... args);

private:
	BaseState*	CurrentState();
	BaseState*	PreviousState();
	void		RemoveState();
	bool		IsEmpty();
	void		Destroy();

private:
	template <typename State>
	void EmplaceBack(std::unique_ptr<State> state);

	template <typename State>
	void EmplaceFront(std::unique_ptr<State> state);

private:
	std::deque<std::unique_ptr<BaseState>> m_states;
};


/*******************************************************************************************************************
	Default constructor
*******************************************************************************************************************/
template <class Implementer, class BaseState>
FiniteStateMachine<Implementer, BaseState>::FiniteStateMachine()
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
template <class Implementer, class BaseState>
FiniteStateMachine<Implementer, BaseState>::~FiniteStateMachine()
{

}


/*******************************************************************************************************************
	Wrapper function that adds a permanent state (created to hide the implementation of std::unique_ptr)
*******************************************************************************************************************/
template <class Implementer, class BaseState>
template <typename State, typename... Args>
void FiniteStateMachine<Implementer, BaseState>::MakePermanentState(Args... args) {
	EmplaceBack(std::make_unique<State>(args...));
}


/*******************************************************************************************************************
	Wrapper function that adds a temporary state (created to hide the implementation of std::unique_ptr)
*******************************************************************************************************************/
template <class Implementer, class BaseState>
template <typename State, typename... Args>
void FiniteStateMachine<Implementer, BaseState>::MakeTemporaryState(Args... args) {
	EmplaceFront(std::make_unique<State>(args...));
}


/*******************************************************************************************************************
	Function that emplaces a temporary state to the front of the deque (e.g. Pause)
*******************************************************************************************************************/
template <class Implementer, class BaseState>
template <typename State>
void FiniteStateMachine<Implementer, BaseState>::EmplaceFront(std::unique_ptr<State> state) {
	m_states.emplace_front(std::move(state));
}


/*******************************************************************************************************************
	Function that emplaces a permanent state to the back of the deque (e.g. Play)
*******************************************************************************************************************/
template <class Implementer, class BaseState>
template <typename State>
void FiniteStateMachine<Implementer, BaseState>::EmplaceBack(std::unique_ptr<State> state) {
	m_states.emplace_back(std::move(state));
}


/*******************************************************************************************************************
	Function which returns the state at the front of the deque
*******************************************************************************************************************/
template <class Implementer, class BaseState>
BaseState* FiniteStateMachine<Implementer, BaseState>::CurrentState()
{
	return m_states.front().get();
}


/*******************************************************************************************************************
	Function which returns the state at the back of the deque
*******************************************************************************************************************/
template <class Implementer, class BaseState>
BaseState* FiniteStateMachine<Implementer, BaseState>::PreviousState()
{
	return m_states.back().get();
}


/*******************************************************************************************************************
	Function which removes the state at the front of the deque
*******************************************************************************************************************/
template <class Implementer, class BaseState>
void FiniteStateMachine<Implementer, BaseState>::RemoveState()
{
	m_states.pop_front();
}


/*******************************************************************************************************************
	Function which checks if the states deque is empty
*******************************************************************************************************************/
template <class Implementer, class BaseState>
bool FiniteStateMachine<Implementer, BaseState>::IsEmpty()
{
	return m_states.empty();
}

/*******************************************************************************************************************
	Function which checks if the states deque is empty
*******************************************************************************************************************/
template <class Implementer, class BaseState>
void FiniteStateMachine<Implementer, BaseState>::Destroy()
{
	return m_states.clear();
}