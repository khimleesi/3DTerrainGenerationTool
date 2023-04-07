#include "Player.h"
#include "managers/InputManager.h"
#include "managers/ReaderManager.h"
#include "utilities/Tools.h"
#include "managers/AudioManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Player::Player(const std::string& tag, const Transform& transform)
	:	GameObject(tag + ".player", transform),
		m_displayInventory(false),
		m_walking(false),
		m_previousPosition(transform.GetPosition()),
		m_terrain(nullptr),
		m_bound(transform.GetPosition(), glm::vec3(1.0f), glm::vec3(1.0f), true),
		m_inventory(m_tag, "Inventory\\Inventory.png", Transform(glm::vec2(0.0f, 0.0f), glm::vec2(256.0f, 256.0f)))
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Player::~Player()
{

}


/*******************************************************************************************************************
	Static function which creates a player instance from data read in from a file
*******************************************************************************************************************/
Player* Player::Create(const std::string& tag)
{
	Reader::Instance()->OpenForReading("Assets\\Files\\gameObjects.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);

	glm::vec3 position	= glm::vec3(StringToFloat(data[tag]["transform.x"]), StringToFloat(data[tag]["transform.y"]), StringToFloat(data[tag]["transform.z"]));
	glm::vec3 rotation	= glm::vec3(StringToFloat(data[tag]["rotation.x"]), StringToFloat(data[tag]["rotation.y"]), StringToFloat(data[tag]["rotation.z"]));
	glm::vec3 scale		= glm::vec3(StringToFloat(data[tag]["scale.x"]), StringToFloat(data[tag]["scale.y"]), StringToFloat(data[tag]["scale.z"]));

	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return new Player(data[tag]["tag"], Transform(position, rotation, scale));
}


/*******************************************************************************************************************
	Function that updates the player
*******************************************************************************************************************/
void Player::Update()
{
	//--- Store the previous player position every frame, enabling us to stop the player when a collision happens
	//--- I wanted to implement wall sliding but ran out of time so it's not the best AI atm :(
	m_previousPosition = m_transform.GetPosition();
	
	ProcessInput();

	if (m_flashLight) { UpdateFlashlight(); }
	if (m_terrain) { FollowTerrain(); ConstrainBounds(); }
	UpdateInventory();
	
	m_transform.Update();
	m_bound.Update(m_transform.GetPosition(), glm::vec3(1.0f), glm::vec3(1.0f));
}


/*******************************************************************************************************************
	Function that processes input and handles events accordingly
*******************************************************************************************************************/
void Player::ProcessInput()
{
	if (m_walking) { Audio::Instance()->PlayAudio("Walking", 10.0f); }

	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_UP))			{ MoveForward(); }
	//--- Moving backwords temporarily disabled until I have a better AI in place i.e. wall sliding
	//else if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_DOWN))	{ MoveBack(); }
	else															{ m_walking = false; }
	
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_SPACE, false)) {
		(m_flashLight->IsEnabled()) ? m_flashLight->SetEnabled(false) : m_flashLight->SetEnabled(true);
	}

	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_I, false)) { m_displayInventory = !m_displayInventory; }

	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_LEFT))		{ RotateLeft(); }
	if (Input::Instance()->IsKeyPressed(SDL_SCANCODE_RIGHT))	{ RotateRight(); }
}


/*******************************************************************************************************************
	Function that constrains the players movement to within the world bounds
*******************************************************************************************************************/
void Player::ConstrainBounds()
{
	if (m_transform.GetPosition().x >= m_terrain->GetBounds()->maximum.x) { m_transform.SetX(m_terrain->GetBounds()->maximum.x); }
	if (m_transform.GetPosition().x <= m_terrain->GetBounds()->minimum.x) { m_transform.SetX(m_terrain->GetBounds()->minimum.x); }

	if (m_transform.GetPosition().z >= m_terrain->GetBounds()->maximum.z) { m_transform.SetZ(m_terrain->GetBounds()->maximum.z); }
	if (m_transform.GetPosition().z <= m_terrain->GetBounds()->minimum.z) { m_transform.SetZ(m_terrain->GetBounds()->minimum.z); }
}

/*******************************************************************************************************************
	Function that updates the players inventory
*******************************************************************************************************************/
void Player::UpdateInventory()
{
	if (m_displayInventory) { m_inventory.Update(); }
}

/*******************************************************************************************************************
	Function that updates the players flashlight
*******************************************************************************************************************/
void Player::UpdateFlashlight()
{
	if (m_flashLight->IsEnabled()) {
		m_flashLight->SetPosition(m_transform.GetPosition());
		m_flashLight->SetDirection(m_transform.GetForward());
	}
}


/*******************************************************************************************************************
	Function that positions the player according to the height of the terrain
*******************************************************************************************************************/
void Player::FollowTerrain()
{
	m_transform.SetY(m_terrain->GetHeight(m_transform.GetPosition().x, m_transform.GetPosition().z, s_offsetFromGround));
}


/*******************************************************************************************************************
	Function that moves the player forward
*******************************************************************************************************************/
void Player::MoveForward()
{
	m_walking = true;
	m_transform.Move(m_transform.GetForward(), s_defaultMovementSpeed);
}


/*******************************************************************************************************************
	Function that moves the player back
*******************************************************************************************************************/
void Player::MoveBack()
{
	m_walking = true;
	m_transform.Move(m_transform.GetForward(), -s_defaultMovementSpeed);
}


/*******************************************************************************************************************
	Function that rotates the player left
*******************************************************************************************************************/
void Player::RotateLeft()
{
	m_transform.Rotate(0.0f, -s_defaultRotationSpeed, 0.0f);
}


/*******************************************************************************************************************
	Function that rotates the player right
*******************************************************************************************************************/
void Player::RotateRight()
{
	m_transform.Rotate(0.0f, s_defaultRotationSpeed, 0.0f);
}


/*******************************************************************************************************************
	Function that renders the players inventory
*******************************************************************************************************************/
void Player::Render(Shader* shader)
{
	if (m_displayInventory) { m_inventory.Render(shader); }
}


/*******************************************************************************************************************
	Function that enables the player to pickup a game object and adds it to the players inventory
*******************************************************************************************************************/
void Player::PickUp(GameObject* object)
{
	if (object) {
		Audio::Instance()->PlayAudio("Pickup", 1.0f);
		object->SetActive(false);
		m_inventory.Add(object->GetTag());
		m_displayInventory = true;
	}
}


/*******************************************************************************************************************
	Function that stops the players movement
*******************************************************************************************************************/
void Player::Stop()
{
	m_walking = false;
	m_transform.SetPosition(m_previousPosition);
}


/*******************************************************************************************************************
	Function that checks if the player has collected all in-game items
*******************************************************************************************************************/
bool Player::HasCollectedAllItems() const { return m_inventory.IsInventoryFull(); }


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const AABounds3D& Player::GetBound() const	{ return m_bound; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Player::SetGround(Terrain* terrain)		{ m_terrain = terrain; }
void Player::SetFlashlight(Light* flashlight) { m_flashLight = flashlight; m_flashLight->SetEnabled(false); }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const float Player::s_offsetFromGround			= 3.0f;
const float Player::s_defaultMovementSpeed		= 0.25f;
const float Player::s_defaultRotationSpeed		= 1.0f;