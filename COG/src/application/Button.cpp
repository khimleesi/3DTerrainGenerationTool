#include "Button.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"
#include "managers/InputManager.h"
#include "managers/AudioManager.h"
#include "managers/ReaderManager.h"
#include "managers/AudioManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Button::Button(const std::string& tag, const std::string& texture, const Transform& transform)
	:	Interface(tag + ".button", transform),
		m_idle(m_tag, texture + ".idle.png"),
		m_hover(m_tag, texture + ".hover.png"),
		m_bound(m_transform.GetPosition(), m_transform.GetDimensions()),
		m_isClicked(false),
		m_isHovered(false)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Button::~Button()
{

}

/*******************************************************************************************************************
	A static function which creates an instance of a button with data read in from a file
*******************************************************************************************************************/
Button* Button::Create(const std::string& tag) {

	Reader::Instance()->OpenForReading("Assets\\Files\\interfaceObjects.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);

	glm::vec2 position = glm::vec2(StringToFloat(data[tag]["transform.x"]), StringToFloat(data[tag]["transform.y"]));
	glm::vec2 dimension = glm::vec2(StringToFloat(data[tag]["width"]), StringToFloat(data[tag]["height"]));

	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return new Button(data[tag]["tag"], data[tag]["sprite"], Transform(position, dimension));
}


/*******************************************************************************************************************
	A function that renders the button to the screen
*******************************************************************************************************************/
void Button::Render(Shader* shader)
{
	if (m_isActive) {
		if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

			if (!m_isHovered) {
				interfaceShader->SetInstanceData(&m_transform, m_idle.GetTexture());
				m_idle.Render();
			}
			else {
				interfaceShader->SetInstanceData(&m_transform, m_hover.GetTexture());
				m_hover.Render();
			}
		}
	}
}


/*******************************************************************************************************************
	A function that updates the button
*******************************************************************************************************************/
void Button::Update()
{
	if (m_isActive) {
		m_transform.Update();
		m_bound.Update(m_transform.GetPosition(), m_transform.GetDimensions());
		ProcessInput();
	}
	else { m_bound.Disable(); m_isClicked = m_isHovered = false; }
}


/*******************************************************************************************************************
	A function that processes input (distinguishes when button is clicked on or hovered)
*******************************************************************************************************************/
void Button::ProcessInput()
{
	if (Input::Instance()->IsMouseColliding(m_bound)) {

		m_isHovered = true;
		
		if (Input::Instance()->IsMouseButtonPressed(SDL_BUTTON_LEFT, false)) {
			m_isClicked = true;
		}
		else { m_isClicked = false; }
	}
	else { m_isHovered = false; }
}


/*******************************************************************************************************************
	A function that checks if a specific button has been clicked
*******************************************************************************************************************/
bool Button::IsClicked(const std::string& tag)
{
	if (m_tag == tag + ".button" && m_isClicked) {
		Audio::Instance()->PlayAudio("Click", 1.0f);
		return true;
	}

	return false;
}


/*******************************************************************************************************************
	A function that returns true if a button has been clicked
*******************************************************************************************************************/
bool Button::IsClicked() {
	
	if (m_isClicked) {
		Audio::Instance()->PlayAudio("Click", 1.0f);
		return true;
	}
	return false;
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Sprite* Button::GetSprite()					{ return &m_idle; }
const AABounds2D& Button::GetBound() const	{ return m_bound; }
bool Button::IsHovered() const				{ return m_isHovered; }