#include "PlayState.h"
#include "managers/GameManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
PlayState::PlayState(GameState* previousState) 
	:	GameState(previousState),
		m_skybox(nullptr),
		m_terrain(nullptr),
		m_player(nullptr),
		m_mainCamera(nullptr),
		m_minimapCamera(nullptr),
		m_picker(nullptr),
		m_frustum(nullptr),
		m_text(nullptr),
		m_minimapWidget(nullptr),
		m_menuButton(nullptr),
		m_helpButton(nullptr),
		m_lightCount(10),
		m_finalEventIssued(false),
		m_debugMode(false),
		m_wireFrameMode(false),
		m_finishedEvents(false)
{
	Initialize();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
PlayState::~PlayState() 
{
	if (m_menuButton)		{ delete m_menuButton; m_menuButton = nullptr; }
	if (m_helpButton)		{ delete m_helpButton; m_helpButton = nullptr; }
	if (m_minimapWidget)	{ delete m_minimapWidget; m_minimapWidget = nullptr; }
	if (m_text)				{ delete m_text; m_text = nullptr; }

	RemoveFromScene(m_shaders);

	if (m_frustum)	{ delete m_frustum; m_frustum = nullptr; }
	if (m_picker)	{ delete m_picker; m_picker = nullptr; }
	
	RemoveFromScene(m_components);
	//--- Collectables deleted here too incase player closes window before game ends
	RemoveFromScene(m_collectables);
	RemoveFromScene(m_entities);

	if (m_player)	{ delete m_player; m_player = nullptr; }
	if (m_terrain)	{ delete m_terrain; m_terrain = nullptr; }
	if (m_skybox)	{ delete m_skybox; m_skybox = nullptr; }

	RemoveFromScene(m_lights);
}


/*******************************************************************************************************************
	Initialize all start up procedures specific to the play state
*******************************************************************************************************************/
void PlayState::Initialize()
{
	IsAlive() = true;

	LoadLights();
	LoadObjects();
	LoadComponents();
	LoadShaders();
	LoadInterface();
	
	//--- Show startup tooltip / begin state
	Game::Instance()->GetStates()->MakeTemporaryState<BeginState>(this);
}


/*******************************************************************************************************************
	A function that creates and initializes all the shaders within this game state
*******************************************************************************************************************/
void PlayState::LoadShaders()
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
	AddToScene(m_shaders, new EntityShader("entityVertexShader.vert", "entityFragmentShader.frag", m_mainCamera));
	AddToScene(m_shaders, new InterfaceShader("interfaceVertexShader.vert", "interfaceFragmentShader.frag"));
	AddToScene(m_shaders, new TextShader("textVertexShader.vert", "textFragmentShader.frag"));
}


/*******************************************************************************************************************
	A function that creates and initializes all objects within this game state
*******************************************************************************************************************/
void PlayState::LoadObjects()
{
	//--- NOTE
	// Everything is read in from text files
	// Later on my file handling/resource management will be much better when I re-create my engine
	// Not enough time to go back and change it all now but eventually it will be something like
	// ObjectFactory->Create(Resource->Load("SomeObject")) - I got a bit carried away with OpenGL and shaders (kind of the point lol)
	//---

	m_skybox	= new Skybox("Night", "Left", "Right", "Top", "Bottom", "Front", "Back");
	m_terrain	= new Terrain();
	m_player	= Player::Create("Player");

	m_terrain->LoadTerrainBinary("Default");

	//--- Give the player something to walk on
	if (m_terrain) { m_player->SetGround(m_terrain); }

	//--- In this case, I only have one spot light and I'm adding it to the back... hackyish much hehe :)
	//--- Will obviously become a problem later on down the road and not how I'd do it! But I'm only thinking about this game atm
	if (!m_lights.empty()) { m_player->SetFlashlight(m_lights.back()); }

	//--- Add all the entities to the scene (basically just emplace them into the vector so we can loop through 'em later!)
	ReserveMemory(m_entities, s_maxEntities);
	for (unsigned int i = 0; i < s_maxEntities; i++) {
		AddToScene(m_entities, Entity::Create("Object" + std::to_string(i))); 
	}
	
	//--- Add all the collectables to the scene (using a deque so no need to reserve memory like the other containers)
	for (unsigned int i = 0; i < s_maxCollectables; i++) {
		AddToScene(m_collectables, Entity::Create("Collectable" + std::to_string(i)));
		if (i >= 1) { m_collectables[i]->SetActive(false); }
	}
}


/*******************************************************************************************************************
	A function that creates and initializes all components within this game state
*******************************************************************************************************************/
void PlayState::LoadComponents()
{
	//--- Add the camera components to the scene
	ReserveMemory(m_components, s_maxComponents);
	AddToScene(m_components, m_mainCamera = new Camera("MainCamera"));
	AddToScene(m_components, m_minimapCamera = new Camera("MinimapCamera", glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f)));

	//--- Set the main camera relative to the players transformations (FPS game)
	m_mainCamera->SetParent(m_player);

	//--- Create the mouse ray and frustum (these will be components as well, eventually)
	m_picker	= new Picker(m_mainCamera);
	m_frustum	= new Frustum(Screen::Instance()->GetProjectionMatrix(), m_mainCamera->GetViewMatrix());
}


/*******************************************************************************************************************
	A function that creates and initializes all interface objects within this game state
*******************************************************************************************************************/
void PlayState::LoadInterface()
{
	m_text				= new Text("FuturaCM.otf", 32);
	m_minimapWidget		= MinimapWidget::Create("MinimapWidget");
	m_menuButton		= Button::Create("MenuButton");
	m_helpButton		= Button::Create("HelpButton");
}


/*******************************************************************************************************************
	A function that creates and initializes all the lights within this game state
*******************************************************************************************************************/
void PlayState::LoadLights()
{
	//--- Max lights is 15 at present (they are only enabled when we can see them)
	//--- If you look really closely in-game, at certain angles you may see them switch off :P totally intentional of course
	ReserveMemory(m_lights, Shader::MAX_LIGHTS);
	for (unsigned int i = 0; i < Shader::MAX_LIGHTS; i++) {
		AddToScene(m_lights, Light::Create("Light" + std::to_string(i)));
	}
}


/*******************************************************************************************************************
	A function that processes all input within this game state
*******************************************************************************************************************/
void PlayState::ProcessInput()
{
	//--- Zoom in/out of view
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_EQUALS))	{ m_mainCamera->Zoom(-s_defaultCameraZoom); }
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_MINUS))	{ m_mainCamera->Zoom(s_defaultCameraZoom); }

	//--- Remove the play state and go back to the menu if user clicks on menu button
	if (m_menuButton->IsClicked()) {
		Audio::Instance()->StopChannel("Play");
		Audio::Instance()->StopChannel("FinalQuest");
		Game::Instance()->GetStates()->MakePermanentState<MenuState>(this);
		IsActive() = IsAlive() = false;
	}
	
	//--- Show the guide state if the help button is clicked
	if (m_helpButton->IsClicked()) {
		Game::Instance()->GetStates()->MakeTemporaryState<GuideState>(this);
		IsActive() = false;
	}

	//--- NOTE
	// Press 'D' when in debug mode to see what's happening in the shaders underneath the hood!
	// Mainly the normal maps, which were a pain in the arse to get right but I am so happy with how they turned out
	// You can see the margin take affect on both the terrain and objects
	//---
#if COG_DEBUG == 1
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_D, false)) { m_debugMode = !m_debugMode; }
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_W, false)) { m_wireFrameMode = !m_wireFrameMode; }
#endif

}


/*******************************************************************************************************************
	Function that updates everything within the play state
*******************************************************************************************************************/
bool PlayState::Update()
{
	ProcessInput();
	UpdateLights();
	UpdateObjects();
	UpdateComponents();
	UpdateInterface();
	
	//--- If player has completed first quest (collected all diary pages), issue final quest
	if (m_player->HasCollectedAllItems() && !m_finishedEvents) { IssueFinalEvent(); }

	return true;
}


/*******************************************************************************************************************
	Function that renders all play state graphics to the screen
*******************************************************************************************************************/
bool PlayState::Render() {

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

	RenderInterface();

	Screen::Instance()->EndScene();

	return true;
}


/*******************************************************************************************************************
	Function that renders all 3D objects to the screen
*******************************************************************************************************************/
void PlayState::RenderWorld()
{

#if COG_DEBUG == 1
	Screen::Instance()->WireframeMode(m_wireFrameMode);
#endif

	//--- Render the skybox
	Screen::Instance()->CullBackFace(false);
	m_shaders[SHADER_SKYBOX]->Bind();
		m_skybox->Render(m_shaders[SHADER_SKYBOX]);
	m_shaders[SHADER_SKYBOX]->Unbind();
	Screen::Instance()->CullBackFace(true);

	//--- Render to texture (minimap)
	m_minimapWidget->GetMinimap()->GetRenderTarget()->BeginScene(true);
		m_shaders[SHADER_TERRAIN]->Bind();
		m_shaders[SHADER_TERRAIN]->SwapCamera(m_minimapCamera);
			m_terrain->SetMinimapMode(true);
			m_terrain->Render(m_shaders[SHADER_TERRAIN]);
		m_shaders[SHADER_TERRAIN]->Unbind();
	m_minimapWidget->GetMinimap()->GetRenderTarget()->EndScene();

	//--- Render the terrain
	m_shaders[SHADER_TERRAIN]->Bind();
#if COG_DEBUG == 1
	m_shaders[SHADER_TERRAIN]->DebugMode(m_debugMode);
#endif
	m_shaders[SHADER_TERRAIN]->SetLights(m_lights);
	m_shaders[SHADER_TERRAIN]->SwapCamera(m_mainCamera);
		m_terrain->SetMinimapMode(false);
		m_terrain->Render(m_shaders[SHADER_TERRAIN]);
	m_shaders[SHADER_TERRAIN]->Unbind();

	// Render the entities
	m_shaders[SHADER_ENTITY]->Bind();
#if COG_DEBUG == 1
	m_shaders[SHADER_ENTITY]->DebugMode(m_debugMode);
#endif
	m_shaders[SHADER_ENTITY]->SetLights(m_lights);
		for (auto entity : m_entities) {
			//--- Entities are only rendered when within view
			if (m_frustum->IsRectangleInside(
				entity->GetBound().GetPosition(),
				entity->GetBound().GetHalfDimension())) {
				
					entity->Render(m_shaders[SHADER_ENTITY]);
			}
		}
		
		//--- Only if there is still items to be collected do we render them
		if (!m_player->HasCollectedAllItems()) {
			//--- Collectables are only rendered when within view
			if (m_frustum->IsRectangleInside(
				m_collectables.front()->GetBound().GetPosition(),
				m_collectables.front()->GetBound().GetHalfDimension())) {
				
				m_collectables.front()->Render(m_shaders[SHADER_ENTITY]);
			}
		}
	m_shaders[SHADER_ENTITY]->Unbind();
}


/*******************************************************************************************************************
	Function that renders all the 2D objects to the screen
*******************************************************************************************************************/
void PlayState::RenderInterface()
{
	//--- Render the interface objects
	m_shaders[SHADER_INTERFACE]->Bind();
		m_menuButton->Render(m_shaders[SHADER_INTERFACE]);
		m_helpButton->Render(m_shaders[SHADER_INTERFACE]);
		m_minimapWidget->Render(m_shaders[SHADER_INTERFACE]);
		m_player->Render(m_shaders[SHADER_INTERFACE]);
	m_shaders[SHADER_INTERFACE]->Unbind();

	//--- Render the text
	m_shaders[SHADER_TEXT]->Bind();
		if (m_finalEventIssued) {
			m_text->Render(m_shaders[SHADER_TEXT], "Lights remaining: " + std::to_string(m_lightCount), Transform(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
		}
		if (m_menuButton->IsHovered()) {
			m_text->Render(m_shaders[SHADER_TEXT], "Return to main menu. Your game will not be saved.", Transform(glm::vec2(40.0f, 403.0f), glm::vec2(0.8f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
		}
		if (m_helpButton->IsHovered()) {
			m_text->Render(m_shaders[SHADER_TEXT], "Display the guide.", Transform(glm::vec2(40.0f, 376.0f), glm::vec2(0.8f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
		}
#if COG_DEBUG == 1
		//m_text->Render(m_shaders[SHADER_TEXT], "FPS : " + std::to_string(Game::Instance()->GetFramesPerSecond()), Transform(glm::vec2(10.0f, 200.0f), glm::vec2(1.0f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
		//m_text->Render(m_shaders[SHADER_TEXT], "Frame Time : " + std::to_string(Game::Instance()->GetCurrentFrameTime()), Transform(glm::vec2(10.0f, 180.0f), glm::vec2(1.0f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
		//m_text->Render(m_shaders[SHADER_TEXT], "CPU % : " + std::to_string(Game::Instance()->GetMainframePercentage()), Transform(glm::vec2(10.0f, 160.0f), glm::vec2(1.0f)), glm::vec4(1.0, 1.0f, 1.0f, 1.0f));
#endif
	m_shaders[SHADER_TEXT]->Unbind();
}


/*******************************************************************************************************************
	Function that updates all the 3D objects
*******************************************************************************************************************/
void PlayState::UpdateObjects()
{
	//--- Update terrain before player so the player is walking in sync with terrain height
	m_terrain->Update();
	m_player->Update();

	//--- Only update the collectables if there is still collectables to pickup
	if (!m_player->HasCollectedAllItems()) {
			
		//--- Don't update anything unless we can see it
		if (m_frustum->IsRectangleInside(
			m_collectables.front()->GetBound().GetPosition(),
			m_collectables.front()->GetBound().GetHalfDimension())) {

			//--- Check if the mouse ray is colliding and if the user clicks then pickup the item and add to inventory
			if (m_picker->IsColliding(m_collectables.front()->GetBound(), s_maxCollectableRange)) {
				if (Input::Instance()->IsMouseButtonPressed(SDL_BUTTON_LEFT, false)) {
						
					m_player->PickUp(m_collectables.front());
						
					//--- Then remove this collectable from memory as we no longer need it
					delete m_collectables.front(); m_collectables.pop_front();
						
					//--- If there is another collectable then set next collectable to an active state & update it
					if (!m_collectables.empty()) { m_collectables.front()->SetActive(true); }
				}
			}
			if (!m_collectables.empty()) { m_collectables.front()->Update(); }
		}
	}

	for (auto entity : m_entities) {
		
		//--- Don't update anything unless we can see it
		if (m_frustum->IsRectangleInside(
			entity->GetBound().GetPosition(),
			entity->GetBound().GetHalfDimension())) {

			//--- Check if the object is even collidable (some objects are outwith world bounds and so don't need to be checked!)
			if (entity->HasCollisionResponse()) {

				//--- If the player collides then stop movement (set to players previous position)
				//--- (Eventually I'll figure out how to implement wall sliding...)
				if (m_player->GetBound().IsColliding(entity->GetBound())) { m_player->Stop(); }
			}
			entity->Update();
		}
	}
}


/*******************************************************************************************************************
	Function that updates all the game components
*******************************************************************************************************************/
void PlayState::UpdateComponents()
{
	//--- NOTE
	// I realise that not all of these are currently components - this will change
	// I have placed them here as I feel they should be components and so when I come back to my
	// engine I can remember to make them so. I'm trying to find a nice way to do it first :)
	//---

	//--- Set the minimap camera to the players position with an offset in the Y coord
	m_minimapCamera->SetPosition(glm::vec3(m_player->GetTransform()->GetPosition().x, 100.0f, m_player->GetTransform()->GetPosition().z));

	//--- Update the cameras
	for (auto component : m_components) { component->Update(); }

	//--- Update the mouse ray - must be done after camera updates
	m_picker->Update();

	//--- Create our new frustum every frame - must be done at the end of all 3D objects updates
	m_frustum->Update(Screen::Instance()->GetProjectionMatrix(), m_mainCamera->GetViewMatrix());
}


/*******************************************************************************************************************
	Function that updates all the lights
*******************************************************************************************************************/
void PlayState::UpdateLights()
{
	//--- Loop through all the lights and only compute lighting calculations in shader if they are within view
	//--- otherwise, disable them when they fall outwith the frustum
	//--- I surround the lights within a temporary radius for now - this can be improved later
	for (auto light : m_lights) {
		if (m_frustum->IsSphereInside(light->GetPosition(), s_maxLightRadius) && light->GetMargin() != 0.0f) {
			if (light->IsOfType(Light::LIGHT_POINT)) { light->SetEnabled(true); }
		}
		else { if (light->IsOfType(Light::LIGHT_POINT)) { light->SetEnabled(false); } }
	}
}


/*******************************************************************************************************************
	Function that updates all the 2D objects
*******************************************************************************************************************/
void PlayState::UpdateInterface()
{
	m_menuButton->Update();
	m_helpButton->Update();
	m_minimapWidget->Update();
}


/*******************************************************************************************************************
	Function that issues the final quest of the game
*******************************************************************************************************************/
void PlayState::IssueFinalEvent()
{
	//--- If we get to this stage, the player is at the final event within the game
	for (auto light : m_lights) {
		
		//--- Change the fancy new graphics and sounds only once when event is activated
		if (light->IsOfType(Light::LIGHT_DIRECTION) && !m_finalEventIssued) {
			
			//--- Play scary quest music and stop previous calming music
			Audio::Instance()->PlayAudio("FinalQuest", 1.0f);
			Audio::Instance()->StopChannel("Play");
			
			//--- Change the directional light to dark red (ooooh)
			light->SetAmbient(glm::vec3(0.4f, 0.1f, 0.1f));
			light->SetDiffuse(glm::vec3(0.4f, 0.0f, 0.0f));
			light->SetSpecular(glm::vec3(0.4f, 0.0f, 0.0f));
			
			//--- Set the event to true so we don't do all this again
			m_finalEventIssued = true;
		}
		
		//--- Now go through all the point lights and set their new linear attenuation to a pulsing effect
		//--- If the lights aren't within the viewing frustum we skip this step entirely until they are within view
		if (light->IsOfType(Light::LIGHT_POINT) && light->GetConstant() > 0.5f && light->IsEnabled()) {
			
			light->SetLinear(abs(cos(SDL_GetTicks() / s_linearPulseAmount)));
			
			//--- Create a temporary bound for this light
			AABounds3D bound(light->GetPosition(), glm::vec3(5.0f), glm::vec3(1.0f), true);
			
			//--- Check if the mouse ray collides with it
			if (m_picker->IsColliding(bound, light->GetMargin())) {

				//--- If the user clicks on the light, turn it off forever and reduce the light count
				if (Input::Instance()->IsMouseButtonPressed(SDL_BUTTON_LEFT, false)) {
					light->SetMargin(0.0f);
					light->SetEnabled(false);
					m_lightCount--;
				}
			}
		}
	}
	
	//--- Finally if we reach 0 lights it means the player has finished the game, so show the end state
	if (m_lightCount == 0) {
		Game::Instance()->GetStates()->MakeTemporaryState<EndState>(this);

		//--- PlayState still alive at this point as they may want to return to it
		IsActive() = false;
		m_finishedEvents = true;
	}
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const unsigned int PlayState::s_maxEntities		= 47;
const unsigned int PlayState::s_maxShaders		= 5;
const unsigned int PlayState::s_maxCollectables = 8;
const unsigned int PlayState::s_maxComponents	= 2;

const float PlayState::s_maxLightRadius			= 20.0f;
const float PlayState::s_linearPulseAmount		= 360.0f;
const float PlayState::s_maxCollectableRange	= 50.0f;
const float PlayState::s_defaultCameraZoom		= 3.0f;