#pragma once

/*******************************************************************************************************************
	GameManager.h, GameManager.cpp
	Created by Kim Kane
	Last updated: 15/04/2018

	Singleton class that handles the setup, run and shutdown of the main game.

	[Features]
	Supports game states using a finite state machine.
	Performance trackers - support for FPS, frame time, update time, delta time and CPU usage.
	
	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <string>

#include "utilities/Singleton.h"

#include "managers/ScreenManager.h"
#include "managers/InputManager.h"
#include "managers/AudioManager.h"
#include "managers/InterfaceManager.h"
#include "managers/ResourceManager.h"
#include "managers/FileManager.h"

#include "application/states/GameState.h"
#include "application/states/MenuState.h"
#include "application/states/StoryState.h"
#include "application/states/GuideState.h"
#include "application/states/CreditsState.h"
#include "application/states/EndState.h"
#include "application/states/BeginState.h"
#include "application/states/EditState.h"
#include "application/states/PlayState.h"

#include "application/states/FiniteStateMachine.h"
#include "utilities/Timer.h"
#include "utilities/Timestep.h"
#include "utilities/MainframeTracker.h"

class GameManager {

public:
	friend class Singleton<GameManager>;
	
private:
	typedef FiniteStateMachine<GameManager, GameState> GameStates;

public:
	void Initialize(const std::string& title, bool fullScreen, bool coreMode, bool vSync);
	void Shutdown();
	void Run();

public:
	GameStates*		GetStates();

public:
	unsigned int	GetFramesPerSecond() const;
	float			GetCurrentFrameTime() const;
	int				GetMainframePercentage();
	float			GetDeltaTime() const;

private:
	void LoadAudio();

private:
	Timestep BeginTime();

private:
	bool HasWindowClosed();
	void Update();
	void Render();
	void ConcludeTime();
	void CheckGameStateStatus();
	void StopSystemTrackers();

private:
	GameManager();
	GameManager(const GameManager&)				= delete;
	GameManager& operator=(const GameManager&)	= delete;

private:
	bool				m_endGame;
	GameStates			m_gameStates;
	Timer				m_gameTimer;
	Timestep			m_timestep;
	MainframeTracker	m_mainframeTracker;
	bool				m_hasLoaded;
};

typedef Singleton<GameManager> Game;