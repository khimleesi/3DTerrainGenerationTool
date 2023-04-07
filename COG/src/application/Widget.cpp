#include "Widget.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"
#include "managers/InputManager.h"
#include "managers/ReaderManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Widget::Widget(const std::string& tag, const std::string& texture, const Transform& transform, const glm::vec2& togglePosition)
	:	Interface(tag + ".widget", transform),
		m_sprite(m_tag + ".border", texture + ".border.png"),
		m_close(m_tag + ".close", texture + ".close", Transform(glm::vec2(m_transform.GetPosition().x + s_defaultOffset, m_transform.GetPosition().y + s_defaultOffset), glm::vec2(s_defaultCloseDimensions))),
		m_open(m_tag + ".open", texture + ".open", Transform(togglePosition, glm::vec2(s_defaultOpenDimensions)))
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Widget::~Widget()
{

}


/*******************************************************************************************************************
	A static function used to create an instance of a widget with data read from a file
*******************************************************************************************************************/
Widget* Widget::Create(const std::string& tag)
{
	Reader::Instance()->OpenForReading("Assets\\Files\\interfaceObjects.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);

	glm::vec2 position	= glm::vec2(StringToFloat(data[tag]["transform.x"]), StringToFloat(data[tag]["transform.y"]));
	glm::vec2 dimension = glm::vec2(StringToFloat(data[tag]["width"]), StringToFloat(data[tag]["height"]));
	glm::vec2 toggle	= glm::vec2(StringToFloat(data[tag]["toggle.x"]), StringToFloat(data[tag]["toggle.y"]));
	
	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return new Widget(data[tag]["tag"], data[tag]["sprite"], Transform(position, dimension), toggle);
}


/*******************************************************************************************************************
	A function that updates the minimap widget, providing it is active
*******************************************************************************************************************/
void Widget::Update()
{
	if (m_isActive) {
		m_transform.Update();
		m_close.Update();
	}
	else { m_open.Update(); }
	
	ProcessInput();
}


/*******************************************************************************************************************
	A function that processes all input related to the minimap widget i.e. toggle open/close
*******************************************************************************************************************/
void Widget::ProcessInput()
{
	if (m_close.IsActive()) {
		if (m_close.IsClicked()) {

			m_close.SetActive(false);
			m_open.SetActive(true);
			m_close.Update();

			m_isActive = false;
		}
	}

	if (m_open.IsActive()) {
		if (m_open.IsClicked()) {

			m_close.SetActive(true);
			m_open.SetActive(false);
			m_open.Update();

			m_isActive = true;
		}
	}
}


/*******************************************************************************************************************
	A function that renders the widget, providing it is active
*******************************************************************************************************************/
void Widget::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		interfaceShader->SetInstanceData(&m_transform, m_sprite.GetTexture());
		
		if (m_isActive) {
			m_sprite.Render();
			m_close.Render(shader);
		}
		else { m_open.Render(shader); }
	}
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const float Widget::s_defaultOffset				= 233.0f;
const float Widget::s_defaultCloseDimensions	= 16.0f;
const float Widget::s_defaultOpenDimensions		= 32.0f;