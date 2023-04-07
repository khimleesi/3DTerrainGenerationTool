#include "EndState.h"
#include "managers/GameManager.h"
#include "managers/AudioManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
EndState::EndState(GameState* previousState)
	:	GameState(previousState),
		m_shader(nullptr),
		m_background(nullptr),
		m_menu(nullptr),
		m_play(nullptr)
{
	Initialize();
}


/*******************************************************************************************************************
Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
EndState::~EndState() {

	if (m_play)			{ delete m_play; m_play = nullptr; }
	if (m_menu)			{ delete m_menu; m_menu = nullptr; }
	if (m_background)	{ delete m_background; m_background = nullptr; }
	if (m_shader)		{ delete m_shader; m_shader = nullptr; }
}


/*******************************************************************************************************************
Initialize all start up procedures specific to this state
*******************************************************************************************************************/
bool EndState::Initialize() {

	IsActive() = IsAlive() = true;

	LoadShaders();
	LoadInterface();

	return true;
}


/*******************************************************************************************************************
A function that creates and initializes all shaders within this game state
*******************************************************************************************************************/
void EndState::LoadShaders()
{
	m_shader = new InterfaceShader("interfaceVertexShader.vert", "interfaceFragmentShader.frag");
}


/*******************************************************************************************************************
A function that creates and initializes all interface objects within this game state
*******************************************************************************************************************/
void EndState::LoadInterface()
{
	m_background = new Background();
	m_background->Create("End", Transform(glm::vec2(0.0f, 0.0f), glm::vec2(1024, 768)), "Backgrounds\\End.png");
	m_play			= Button::Create("ResumeButton");
	m_menu			= Button::Create("EndButton");
}


/*******************************************************************************************************************
A function that processes all input within this game state
*******************************************************************************************************************/
void EndState::ProcessInput()
{
	if (m_play->IsClicked()) {
		m_previousState->IsActive() = true;
		IsActive() = IsAlive() = false;
	}
	m_play->Update();

	if (m_menu->IsClicked()) {
		Audio::Instance()->StopChannel("Play");
		Audio::Instance()->StopChannel("FinalQuest");
		m_previousState->IsAlive() = false;
		Game::Instance()->GetStates()->MakePermanentState<MenuState>(this);
		IsActive() = IsAlive() = false;
	}
	m_menu->Update();
}


/*******************************************************************************************************************
Function that updates everything within this state
*******************************************************************************************************************/
bool EndState::Update() {

	m_background->Update();

	ProcessInput();

	return true;
}


/*******************************************************************************************************************
Function that renders all this states graphics to the screen
*******************************************************************************************************************/
bool EndState::Render() {

	Screen::Instance()->BeginScene(0.0f, 0.0f, 0.0f);
	Screen::Instance()->PerspectiveView(false);
	Screen::Instance()->EnableBlending(true);
	Screen::Instance()->EnableDepth(false);
	Screen::Instance()->CullBackFace(false);

	m_shader->Bind();
		m_background->Render(m_shader);
		m_menu->Render(m_shader);
		m_play->Render(m_shader);
	m_shader->Unbind();

	Screen::Instance()->EndScene();

	return true;
}