#include "EditState.h"
#include "managers/GameManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
EditState::EditState(GameState* previousState) 
	:	GameState(previousState),
		m_skybox(nullptr),
		m_player(nullptr),
		m_mainCamera(nullptr),
		m_terrain(nullptr),
		m_fogType(shader_constants::FOG_EXP),
		m_fogDensity(shader_constants::FOG_DENSITY),
		m_fogColor(shader_constants::FOG_COLOR),
		m_isFogRanged(shader_constants::IS_FOG_RANGED),
		m_isSkyboxTinted(shader_constants::SKYBOX_TINTED),
		m_skyboxTintColor(shader_constants::SKYBOX_TINT_COLOR),
		m_tintBegin(shader_constants::SKYBOX_TINT_BEGIN),
		m_tintEnd(shader_constants::SKYBOX_TINT_END),
		m_debugMode(false),
		m_wireFrameMode(false),
		m_editingMode(false)
{
	Initialize();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
EditState::~EditState() 
{
	RemoveFromScene(m_shaders);
	RemoveFromScene(m_components);

	if (m_player)	{ delete m_player; m_player = nullptr; }
	if (m_skybox)	{ delete m_skybox; m_skybox = nullptr; }

	RemoveFromScene(m_lights);
}


/*******************************************************************************************************************
	Initialize all start up procedures specific to the play state
*******************************************************************************************************************/
void EditState::Initialize()
{
	IsActive() = IsAlive() = true;

	LoadLights();
	LoadObjects();
	LoadComponents();
	LoadShaders();
}


/*******************************************************************************************************************
	A function that creates and initializes all the shaders within this game state
*******************************************************************************************************************/
void EditState::LoadShaders()
{
	//--- NOTE
	// I am still trying to find a good way to organise shaders atm
	// I use polymorphism and then downcast them when rendering, but I realise downcasting is fairly expensive to do
	// So I'm currently researching better ways - I can't ever have just one shader, I like them too much lol
	// Thought it was worth a mention - I'll be changing this in future :)
	//---

	ReserveMemory(m_shaders, s_maxShaders);
	AddToScene(m_shaders, new SkyboxShader("skyboxVertexShader.vert", "skyboxFragmentShader.frag", m_mainCamera));
	AddToScene(m_shaders, new TerrainShader("terrainVertexShader.vert", "terrainFragmentShader.frag", m_mainCamera));
}


/*******************************************************************************************************************
	A function that creates and initializes all objects within this game state
*******************************************************************************************************************/
void EditState::LoadObjects()
{
	m_skybox	= new Skybox("Night", "Left", "Right", "Top", "Bottom", "Front", "Back");

	m_player	= SamplePlayer::Create("SamplePlayer");

	m_terrain = std::make_unique<Terrain>();
	
	m_terrain->LoadTerrainBinary("Default");

	//--- Give the player something to walk on
	if (m_terrain) { m_player->SetGround(m_terrain.get()); }
}


/*******************************************************************************************************************
	A function that creates and initializes all components within this game state
*******************************************************************************************************************/
void EditState::LoadComponents()
{
	//--- Add the camera components to the scene
	ReserveMemory(m_components, s_maxComponents);
	AddToScene(m_components, m_mainCamera = new Camera("MainCamera"));

	//--- Set the main camera relative to the players transformations (FPS game)
	m_mainCamera->SetParent(m_player);
}


/*******************************************************************************************************************
	A function that creates and initializes all the lights within this game state
*******************************************************************************************************************/
void EditState::LoadLights()
{
	//--- Max lights is 15 at present (they are only enabled when we can see them)
	//--- If you look really closely in-game, at certain angles you may see them switch off :P totally intentional of course
	ReserveMemory(m_lights, Shader::MAX_LIGHTS);
	AddToScene(m_lights, Light::Create("Light0"));
}


/*******************************************************************************************************************
	A function that processes all input within this game state
*******************************************************************************************************************/
void EditState::ProcessInput()
{
	//--- Zoom in/out of view
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_EQUALS))	{ m_mainCamera->Zoom(-s_defaultCameraZoom); }
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_MINUS))	{ m_mainCamera->Zoom(s_defaultCameraZoom); }
	
	if (m_editingMode) {

		if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_UP)) { m_mainCamera->Move(m_mainCamera->GetForward(), 1.0f); }
		if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_DOWN)) { m_mainCamera->Move(m_mainCamera->GetForward(), -1.0f); }
		if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_LEFT)) { m_mainCamera->Move(m_mainCamera->GetRight(), -1.0f); }
		if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_RIGHT)) { m_mainCamera->Move(m_mainCamera->GetRight(), 1.0f); }
		m_mainCamera->Rotate(0.0f, Input::Instance()->GetMouseMotion().x / 10.0f,0.0f);
		m_mainCamera->Rotate(Input::Instance()->GetMouseWheel().y * 2.0f, 0.0f, 0.0f);
	}
}


/*******************************************************************************************************************
	Function that updates everything within the play state
*******************************************************************************************************************/
bool EditState::Update()
{
	ProcessInput();
	UpdateObjects();
	UpdateComponents();

	return true;
}


/*******************************************************************************************************************
	Function that renders all play state graphics to the screen
*******************************************************************************************************************/
bool EditState::Render() {

	GUI::Instance()->NewFrame();

	static char tag[15]			= "Default";
	static char heightmap[15]	= "Default";
	static char base[15]		= "Base";
	static char red[15]			= "Red";
	static char green[15]		= "Green";
	static char blue[15]		= "Blue";
	static char blendmap[15]	= "Blendmap";
	
	static bool previewTextures = false;

	static glm::vec3 position	= m_terrain->GetTransform()->GetPosition();
	static glm::vec3 rotation	= m_terrain->GetTransform()->GetRotation();
	static glm::vec3 scale		= m_terrain->GetTransform()->GetScale();
	static glm::vec3 minimum	= m_terrain->GetBounds()->minimum;
	static glm::vec3 maximum	= m_terrain->GetBounds()->maximum;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open Terrain Binary")) { m_terrain->LoadTerrainBinaryFromDialog(); }
			if (ImGui::MenuItem("Save Terrain Binary")) {
				m_terrain->SaveTerrainViaDialog(tag,
												Transform(position, rotation, scale),
												TexturePack(base, red, green, blue, blendmap),
												TexturePack(base, red, green, blue),
												{ {minimum}, {maximum} });
			}

			if (ImGui::MenuItem("Save Raw Heightmap Data")) {
				if (m_terrain->SaveRawHeightMapData(tag,
					Transform(position, rotation, scale),
					TexturePack(base, red, green, blue, blendmap),
					TexturePack(base, red, green, blue),
					heightmap, { {minimum}, {maximum} }))
				{
					GUI::Instance()->Popup("File saved!", "Your terrain was saved successfully.");
				}
				else { GUI::Instance()->Popup("Error saving file!", "Your terrain was not saved.  Make sure binary file doesn't already exist."); }
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Main Menu")) {
				Audio::Instance()->StopChannel("Editor");
				Game::Instance()->GetStates()->MakePermanentState<MenuState>(this);
				IsActive() = IsAlive() = false;
			}

			if (ImGui::MenuItem("Quit")) {
				Input::Instance()->SetWindowClosed(true);
				IsActive() = IsAlive() = false;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera")) {
			if (ImGui::MenuItem("Edit Mode", nullptr, m_editingMode)) {
				m_editingMode = !m_editingMode;
				if (m_editingMode) {
					m_mainCamera->SetParent(nullptr);
					m_mainCamera->SetPosition(glm::vec3(0.0f, 25.0f, 0.0f));
					m_mainCamera->SetRotation(glm::vec3(45.0f, 0.0f, 0.0f));
				}
				else { m_mainCamera->SetParent(m_player); }
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Render Mode")) {
			if (ImGui::MenuItem("Render Wireframe", nullptr, m_wireFrameMode)) { m_wireFrameMode = !m_wireFrameMode; }
			if (ImGui::MenuItem("Render Normals", nullptr, m_debugMode)) { m_debugMode = !m_debugMode; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About")) { ImGui::Text("Created by Kim Kane. 2019."); ImGui::EndMenu(); }
		ImGui::EndMainMenuBar();
	}
	
	GUI::Instance()->BeginWindow("Terrain Editor | Current Terrain: " + m_terrain->GetTag());
	
	ImGui::Text("Tag");
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Change the terrain tag name."); }
	ImGui::SameLine();
	ImGui::InputText("##Tag", tag, IM_ARRAYSIZE(tag), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::Separator();

	ImGui::Text("Heightmap");
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The heightmap file associated with this terrain. Only applicable when saving raw data."); }
	ImGui::SameLine();
	ImGui::InputText("##Heightmap", heightmap, IM_ARRAYSIZE(heightmap), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::Separator();

	GUI::Instance()->CreateDragVec3("Position", "Change the position of the terrain.", position); m_terrain->GetTransform()->SetPosition(position); ImGui::Separator();
	GUI::Instance()->CreateDragVec3("Rotation", "Change the rotation of the terrain.", rotation); m_terrain->GetTransform()->SetRotation(rotation); ImGui::Separator();
	GUI::Instance()->CreateDragVec3("Scale", "Change the scale of the terrain.", scale); m_terrain->GetTransform()->SetScale(scale); ImGui::Separator();
	GUI::Instance()->CreateDragVec3("Minimum Bounds", "The minimum X, Y and Z position player/camera can view.", minimum); ImGui::Separator();
	GUI::Instance()->CreateDragVec3("Maximum Bounds", "The maximum X, Y and Z position player/camera can view.", maximum); m_terrain->SetBounds(minimum, maximum); ImGui::Separator();

	ImGui::Text("Textures");
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Change the terrain textures. Textures must be located in Textures\\Terrain folder and be .png format."); }

	ImGui::InputText("##BaseTexture", base, IM_ARRAYSIZE(base), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::InputText("##RedTexture", red, IM_ARRAYSIZE(red), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::InputText("##GreenTexture", green, IM_ARRAYSIZE(green), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::InputText("##BlueTexture", blue, IM_ARRAYSIZE(blue), ImGuiInputTextFlags_CharsNoBlank);
	ImGui::InputText("##BlendmapTexture", blendmap, IM_ARRAYSIZE(blendmap), ImGuiInputTextFlags_CharsNoBlank);

	ImGui::Checkbox("Preview Textures?", &previewTextures);
	if (previewTextures) { m_terrain->GetDiffuseTexturePack()->LoadDiffuse(base, red, green, blue, blendmap); }
	ImGui::Separator();

	ImGui::Text("Fog");
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Change the terrain fog shader effect."); }
	const char* fogTypes[] = { "Linear", "Exponential", "Exponential Squared" };
	ImGui::Combo("Type", &m_fogType, fogTypes, IM_ARRAYSIZE(fogTypes));
	ImGui::DragFloat("Density", &m_fogDensity, 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::ColorEdit4("Color", &m_fogColor[0]);
	ImGui::Checkbox("Ranged?", &m_isFogRanged);
	ImGui::Separator();

	ImGui::Text("Skybox");
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Change the skybox tint shader effect."); }
	ImGui::Checkbox("Enable Tint?", &m_isSkyboxTinted);
	ImGui::ColorEdit4("Tint Color", &m_skyboxTintColor[0]);
	ImGui::DragFloat("Tint Begin", &m_tintBegin, 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Tint End", &m_tintEnd, 0.01f, 0.0f, 100.0f, "%.2f");

	ImGui::End();
	ImGui::Render();

	//--- Render all 3D objects
	Screen::Instance()->BeginScene(0.0f, 0.0f, 0.0f);
	Screen::Instance()->PerspectiveView(true);
	Screen::Instance()->EnableBlending(false);
	Screen::Instance()->EnableDepth(true);

	RenderWorld();

	//--- Render all 2D objects
	Screen::Instance()->PerspectiveView(false);
	Screen::Instance()->EnableBlending(true);
	Screen::Instance()->EnableDepth(false);
	Screen::Instance()->CullBackFace(false);

	GUI::Instance()->Render();
	Screen::Instance()->EndScene();

	return true;
}


/*******************************************************************************************************************
	Function that renders all 3D objects to the screen
*******************************************************************************************************************/
void EditState::RenderWorld()
{
	Screen::Instance()->WireframeMode(m_wireFrameMode);

	//--- Render the skybox
	Screen::Instance()->CullBackFace(false);

	m_shaders[SHADER_SKYBOX]->Bind();
	m_shaders[SHADER_SKYBOX]->SetSkyboxData(m_isSkyboxTinted, m_tintBegin, m_tintEnd, m_skyboxTintColor);
	m_skybox->Render(m_shaders[SHADER_SKYBOX]);
	m_shaders[SHADER_SKYBOX]->Unbind();
	Screen::Instance()->CullBackFace(true);

	//--- Render the terrain
	m_shaders[SHADER_TERRAIN]->Bind();
	m_shaders[SHADER_TERRAIN]->DebugMode(m_debugMode);
	m_shaders[SHADER_TERRAIN]->SetLights(m_lights);
	m_shaders[SHADER_TERRAIN]->SwapCamera(m_mainCamera);
	m_shaders[SHADER_TERRAIN]->SetFogData(m_fogType, m_isFogRanged, m_fogDensity, m_fogColor);
		m_terrain->SetMinimapMode(false);
		m_terrain->Render(m_shaders[SHADER_TERRAIN]);
	m_shaders[SHADER_TERRAIN]->Unbind();

}


/*******************************************************************************************************************
	Function that updates all the 3D objects
*******************************************************************************************************************/
void EditState::UpdateObjects()
{
	//--- Update terrain before player so the player is walking in sync with terrain height
	m_terrain->Update();
	if (!m_editingMode) { m_player->Update(); }
}


/*******************************************************************************************************************
	Function that updates all the game components
*******************************************************************************************************************/
void EditState::UpdateComponents()
{
	//--- NOTE
	// I realise that not all of these are currently components - this will change
	// I have placed them here as I feel they should be components and so when I come back to my
	// engine I can remember to make them so. I'm trying to find a nice way to do it first :)
	//---

	//--- Update the cameras
	for (auto component : m_components) { component->Update(); }
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const unsigned int EditState::s_maxShaders		= 5;
const unsigned int EditState::s_maxComponents	= 2;
const float EditState::s_defaultCameraZoom		= 3.0f;