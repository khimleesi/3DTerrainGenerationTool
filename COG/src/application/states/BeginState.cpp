#include "BeginState.h"
#include "managers/GameManager.h"
#include "managers/AudioManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
BeginState::BeginState(GameState* previousState)
	:	GameState(previousState),
		m_shader(nullptr),
		m_background(nullptr),
		m_backButton(nullptr),
		m_beginButton(nullptr)
{
	Initialize();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
BeginState::~BeginState() {

	if (m_beginButton)	{ delete m_beginButton; m_beginButton = nullptr; }
	if (m_backButton)	{ delete m_backButton; m_backButton = nullptr; }
	if (m_background)	{ delete m_background; m_background = nullptr; }
	if (m_shader)		{ delete m_shader; m_shader = nullptr; }
}


/*******************************************************************************************************************
	Initialize all start up procedures specific to this state
*******************************************************************************************************************/
bool BeginState::Initialize() {

	IsActive() = IsAlive() = true;

	LoadShaders();
	LoadInterface();

	return true;
}


/*******************************************************************************************************************
	A function that creates and initializes all shaders within this game state
*******************************************************************************************************************/
void BeginState::LoadShaders()
{
	m_shader = new InterfaceShader("interfaceVertexShader.vert", "interfaceFragmentShader.frag");
}


/*******************************************************************************************************************
	A function that creates and initializes all interface objects within this game state
*******************************************************************************************************************/
void BeginState::LoadInterface()
{
	m_background = new Background();
	m_background->Create("Begin", Transform(glm::vec2(0.0f, 0.0f), glm::vec2(1024, 768)), "Backgrounds\\Begin.png");
	m_backButton	= Button::Create("BackButton");
	m_beginButton	= Button::Create("BeginButton");
}


/*******************************************************************************************************************
	A function that processes all input within this game state
*******************************************************************************************************************/
void BeginState::ProcessInput()
{
	if (m_beginButton->IsClicked()) {
		m_previousState->IsActive() = true;
		IsActive() = IsAlive() = false;
	}
	m_beginButton->Update();

	if (m_backButton->IsClicked()) {
		Audio::Instance()->StopChannel("Play");
		m_previousState->IsActive() = m_previousState->IsAlive() = false;
		Game::Instance()->GetStates()->MakePermanentState<MenuState>(this);
		IsActive() = IsAlive() = false;
	}
	m_backButton->Update();
}


/*******************************************************************************************************************
	Function that updates everything within this state
*******************************************************************************************************************/
bool BeginState::Update() {
	
	m_background->Update();

	ProcessInput();

	return true;
}


/*******************************************************************************************************************
	Function that renders all this states graphics to the screen
*******************************************************************************************************************/
bool BeginState::Render() {
	
	Screen::Instance()->BeginScene(0.0f, 0.0f, 0.0f);
	Screen::Instance()->PerspectiveView(false);
	Screen::Instance()->EnableBlending(true);
	Screen::Instance()->EnableDepth(false);
	Screen::Instance()->CullBackFace(false);

	m_shader->Bind();
		m_background->Render(m_shader);
		m_beginButton->Render(m_shader);
		m_backButton->Render(m_shader);
	m_shader->Unbind();

	Screen::Instance()->EndScene();

	return true;
}