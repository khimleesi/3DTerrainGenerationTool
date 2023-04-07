#include "GameManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
GameManager::GameManager()	
	:	m_endGame(false),
		m_gameTimer(true),
		m_hasLoaded(false),
		m_timestep(0.0f)
{
	COG_LOG("[GAME MANAGER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Function that initializes the game and sets up all default game settings
*******************************************************************************************************************/
void GameManager::Initialize(const std::string& title, bool fullScreen, bool coreMode, bool vSync)
{
	using namespace screen_constants;

	File::Instance()->Initialize("Assets\\Files\\srExtensions.ext");

	//--- Initialize the game window
	Screen::Instance()->Initialize(title, WIDTH, HEIGHT, OPENGL_VERSION, OPENGL_SUBVERSION, fullScreen, coreMode, vSync);

	//--- Initialize the in-game input
	Input::Instance()->Initialize();

	//IMGUI TESTING......................
	GUI::Instance()->Initialize(true, nullptr);
	
	//--- Initialize the in-game audio
	Audio::Instance()->Initialize();

	//--- Load all of the in-game audio files
	LoadAudio();

	//--- Initialize a new Menu State
	m_gameStates.MakeTemporaryState<MenuState>(nullptr);
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
void GameManager::Shutdown()
{
	Resource::Instance()->Shutdown();
	Audio::Instance()->Shutdown();
	GUI::Instance()->Shutdown();
	Input::Instance()->ShutDown();
	Screen::Instance()->ShutDown();
	File::Instance()->Shutdown("Assets\\Files\\srExtensions.ext");

	COG_LOG("[GAME MANAGER SHUT DOWN]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	A function that loads all audio files used within the game
*******************************************************************************************************************/
void GameManager::LoadAudio()
{
	Audio::Instance()->LoadAudio("Editor", "Music\\Editor.mp3", true);
	Audio::Instance()->LoadAudio("Play", "Music\\Play.mp3", true);
	Audio::Instance()->LoadAudio("FinalQuest", "Music\\FinalQuest.mp3", true);

	Audio::Instance()->LoadAudio("Click", "Buttons\\Click.wav", false);
	Audio::Instance()->LoadAudio("Walking", "Player\\Walking.wav", false, 25.0f);
	Audio::Instance()->LoadAudio("Pickup", "Player\\Pickup.wav", false);
}


/*******************************************************************************************************************
	Where the magic happens
*******************************************************************************************************************/
void GameManager::Run()
{
	//--- Start our game timer so we can keep track of FPS, frame times and update times
	m_timestep = BeginTime();

	//--- Begin the madness
	while (!m_endGame) {

		//--- While we have an active game state
		while (m_gameStates.CurrentState()->IsActive()) {

			//--- If the window is closed at any stage break out of this loop
			if (HasWindowClosed()) { break; }
			
			//--- Update the game and process all physics depending on the timestep amount
			Update();

			//--- Render all graphics
			Render();

			//--- Store the timings of this frame and reset the per frame timings
			ConcludeTime();
		}

		//--- Check if we still have a current game state
		CheckGameStateStatus();
	}

	//--- If game has ended, stop any system trackers we have running in the background
	StopSystemTrackers();
}


/*******************************************************************************************************************
	A function that stops any system trackers we have running that need memory cleanup before game shuts down
*******************************************************************************************************************/
void GameManager::StopSystemTrackers()
{
	m_mainframeTracker.Stop();
}


/*******************************************************************************************************************
	A function that checks if we have any game states
*******************************************************************************************************************/
void GameManager::CheckGameStateStatus()
{
	if (!m_gameStates.CurrentState()->IsAlive())	{ m_gameStates.RemoveState(); }
	if (m_gameStates.IsEmpty())						{ m_gameStates.Destroy(); m_endGame = true; }
}


/*******************************************************************************************************************
	A function that begins the game timer and returns a new timestep
*******************************************************************************************************************/
Timestep GameManager::BeginTime()
{	
	//--- Start tracking the mainframe usage
	m_mainframeTracker.Start();

	//--- Start the game timer
	m_gameTimer.Start();

	//--- Set the frame starting point
	m_gameTimer.SetThisFrame(0.0f, m_gameTimer.ElapsedMilliseconds(), 0, 0);

	//--- Create a new timestep and set the initial starting time to the elapsed ms of the game timer
	return Timestep(m_gameTimer.ElapsedMilliseconds());
}


/*******************************************************************************************************************
	A function that processes input and checks if the window has closed any point during the game
*******************************************************************************************************************/
bool GameManager::HasWindowClosed()
{
	//--- Process input events
	Input::Instance()->Update();

	if (Input::Instance()->IsWindowClosed() ||
		Input::Instance()->IsKeyPressed(SDL_SCANCODE_ESCAPE)) {

		//--- If window has closed, kill our game state(s)
		m_gameStates.CurrentState()->IsActive()	= m_gameStates.CurrentState()->IsAlive() = false;
		m_gameStates.PreviousState()->IsActive() = m_gameStates.PreviousState()->IsAlive() = false;

		return true;
	}

	return false;
}


/*******************************************************************************************************************
	A function that updates our game using the timestep calculated
*******************************************************************************************************************/
void GameManager::Update()
{
	//--- Update the mainframe usage
	m_mainframeTracker.Update();

	//--- Update the in-game audio
	Audio::Instance()->Update();

	//--- Default frame time = (MS per second / Default FPS) = (1000.0f / 60.0f) = (approx)16.6ms
	const float defaultFrameTime = Timer::GetDefaultFrameTime();

	//--- Current ticks - returns number of milliseconds since the game timer was initialized
	float currentTicks = m_gameTimer.ElapsedMilliseconds();
	
	//--- Previous ticks - returns number of ticks since the last frame
	float previousTicks	= m_gameTimer.GetThisFrame()->previousTicks;
	
	//--- If (current ticks - previous ticks) is greater than (approx)16.6ms
	if ((currentTicks - previousTicks) > defaultFrameTime) {

		//--- Update the timestep with the current ticks
		m_timestep.Update(currentTicks);
		
		//--- Update the game updates with the delta time calculated
		m_gameStates.CurrentState()->Update();

		//--- Set the update count + 1
		m_gameTimer.GetThisFrame()->updates++;

		//--- Append the previous ticks + (approx)16.6ms
		m_gameTimer.GetThisFrame()->previousTicks += defaultFrameTime;

		//--- Game has been loaded for the first time, now allow rendering 
		//--- (precautionary only - just makes sure objects have updated at least once before graphics being drawn)
		if (!m_hasLoaded) { m_hasLoaded = true; }
	}
}


/*******************************************************************************************************************
	A function that renders our game and determines the frame count & time
*******************************************************************************************************************/
void GameManager::Render()
{
	if (m_hasLoaded) {
		Timer frameTime(true);

		//--- Start the frame timer
		frameTime.Start();

		//--- Render everything within the game
		m_gameStates.CurrentState()->Render();

		//--- Set the frame count + 1
		m_gameTimer.GetThisFrame()->frames++;

		//--- Set the frame time it took to render this frame
		m_gameTimer.SetFrameTime(frameTime.ElapsedMilliseconds());
	}
}


/*******************************************************************************************************************
	A function that concludes the game timings; storing the values of all calculated frame data per second
*******************************************************************************************************************/
void GameManager::ConcludeTime()
{	
	//--- Set to 1.0f
	const float maxSeconds = Timer::GetMaxSeconds();

	//--- Elapsed seconds - returns number of seconds since the game timer was initialized
	float elapsedSeconds = m_gameTimer.ElapsedSeconds();

	//--- Previous seconds - returns number of seconds since the last frame
	float previousSeconds = m_gameTimer.GetThisFrame()->previousSeconds;

	//--- If the (elapsed seconds - previousSeconds) is greated than 1.0f
	if ((elapsedSeconds - previousSeconds) > maxSeconds) {

		//--- Append the previous seconds with 1.0f
		m_gameTimer.GetThisFrame()->previousSeconds += maxSeconds;
		
		//--- Store the frames per this second
		m_gameTimer.SetFramesPerSecond(m_gameTimer.GetThisFrame()->frames);

		//--- Store the updates per this second
		m_gameTimer.SetUpdatesPerSecond(m_gameTimer.GetThisFrame()->updates);
		
		//--- Then reset the frames and updates to 0
		m_gameTimer.GetThisFrame()->frames = 0;
		m_gameTimer.GetThisFrame()->updates = 0;
	}
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
GameManager::GameStates* GameManager::GetStates()		{ return &m_gameStates; }
unsigned int GameManager::GetFramesPerSecond() const	{ return m_gameTimer.GetFramesPerSecond(); }
float GameManager::GetCurrentFrameTime() const			{ return m_gameTimer.GetCurrentFrameTime(); }
int	GameManager::GetMainframePercentage() 				{ return m_mainframeTracker.GetMainframePercentage(); }
float GameManager::GetDeltaTime() const					{ return m_timestep.GetDeltaTime(); }