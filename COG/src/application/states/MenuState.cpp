#include "MenuState.h"
#include "managers/GameManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
MenuState::MenuState(GameState* previousState)
	:	GameState(previousState),
		m_shader(nullptr),
		m_background(nullptr)
{
	Initialize();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
MenuState::~MenuState() {

	if (m_background)	{ delete m_background; m_background = nullptr; }
	if (m_shader)		{ delete m_shader; m_shader = nullptr; }
}


/*******************************************************************************************************************
	Initialize all start up procedures specific to the menu state
*******************************************************************************************************************/
bool MenuState::Initialize() {

	IsActive() = IsAlive() = true;

	LoadShaders();
	LoadInterface();

	return true;
}


/*******************************************************************************************************************
	A function that creates and initializes all shaders within this game state
*******************************************************************************************************************/
void MenuState::LoadShaders()
{
	m_shader = new InterfaceShader("interfaceVertexShader.vert", "interfaceFragmentShader.frag");
}


/*******************************************************************************************************************
	A function that creates and initializes all interface objects within this game state
*******************************************************************************************************************/
void MenuState::LoadInterface()
{
	m_background = new Background();

	//m_background->Create("Menu", Transform(glm::vec2(0.0f, 0.0f), glm::vec2(1024, 768)), "Backgrounds\\Menu.png");
	m_background->Load("Menu");

	GUI::Instance()->CreateButton("Terrain", "Create a Terrain", 496, 50);
	GUI::Instance()->CreateButton("Game", "Begin Sample Game", 496, 50);
	GUI::Instance()->CreateButton("Exit", "Exit", 496, 50);
}


/*******************************************************************************************************************
	A function that processes all input within this game state
*******************************************************************************************************************/
void MenuState::ProcessInput()
{
	if (GUI::Instance()->IsButtonClicked("Terrain"))
	{
		Game::Instance()->GetStates()->MakePermanentState<EditState>(this);
		Audio::Instance()->PlayAudio("Editor", 1.0f);
		IsActive() = IsAlive() = false;
	}

	if (GUI::Instance()->IsButtonClicked("Game"))
	{
		Game::Instance()->GetStates()->MakePermanentState<PlayState>(this);
		Audio::Instance()->StopChannel("Editor");
		Audio::Instance()->PlayAudio("Play", 1.0f);
		IsActive() = IsAlive() = false;
	}

	if (GUI::Instance()->IsButtonClicked("Exit"))
	{
		Input::Instance()->SetWindowClosed(true);
		IsActive() = IsAlive() = false;
	}
}


/*******************************************************************************************************************
	Function that updates everything within the menu state
*******************************************************************************************************************/
bool MenuState::Update() {
	
	m_background->Update();
	
	return true;
}


/*******************************************************************************************************************
	Function that renders all menu state graphics to the screen
*******************************************************************************************************************/
bool MenuState::Render() {
	
	GUI::Instance()->NewFrame();
	ImGui::SetNextWindowSize(ImVec2(Screen::Instance()->GetWidth() / 2, Screen::Instance()->GetHeight() / 3), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("COG : Game Engine and Terrain Generation Tool", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::NewLine();
	ProcessInput();
	ImGui::NewLine();
	ImGui::Text("Created by Kim Kane.");
	ImGui::End();
	ImGui::Render();

	Screen::Instance()->BeginScene(0.0f, 0.0f, 0.0f);
	Screen::Instance()->PerspectiveView(false);
	Screen::Instance()->EnableBlending(true);
	Screen::Instance()->EnableDepth(false);
	Screen::Instance()->CullBackFace(false);

	m_shader->Bind();
		m_background->Render(m_shader);
	m_shader->Unbind();

	GUI::Instance()->Render();

	Screen::Instance()->EndScene();

	return true;
}