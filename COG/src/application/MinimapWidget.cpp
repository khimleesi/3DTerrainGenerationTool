#include "MinimapWidget.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"
#include "managers/InputManager.h"
#include "managers/ScreenManager.h"
#include "managers/ReaderManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
MinimapWidget::MinimapWidget(const std::string& tag, const std::string& texture, const Transform& transform)
	:	Widget(tag, texture, transform, glm::vec2(Screen::Instance()->GetWidth() - 50.0f, 15.0f)),
		m_minimap(m_tag + ".minimap", Transform(glm::vec2(Screen::Instance()->GetWidth() - 252.0f, 3.0f), glm::vec2(249.0f, 217.0f)))
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
MinimapWidget::~MinimapWidget()
{

}


/*******************************************************************************************************************
	A static function used to create an instance of a minimap widget with data read from a file
*******************************************************************************************************************/
MinimapWidget* MinimapWidget::Create(const std::string& tag)
{
	Reader::Instance()->OpenForReading("Assets\\Files\\interfaceObjects.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);

	glm::vec2 position	= glm::vec2(StringToFloat(data[tag]["transform.x"]), StringToFloat(data[tag]["transform.y"]));
	glm::vec2 dimension = glm::vec2(StringToFloat(data[tag]["width"]), StringToFloat(data[tag]["height"]));

	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return new MinimapWidget(data[tag]["tag"], data[tag]["sprite"], Transform(position, dimension));
}


/*******************************************************************************************************************
	A function that updates the minimap widget, providing it is active
*******************************************************************************************************************/
void MinimapWidget::Update()
{
	Widget::Update();
}


/*******************************************************************************************************************
	A function that renders the minimap widget, providing it is active
*******************************************************************************************************************/
void MinimapWidget::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		interfaceShader->SetInstanceData(&m_transform, nullptr);
		
		Widget::Render(shader);
		if (Widget::IsActive()) { m_minimap.Render(shader); }
	}
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Minimap* MinimapWidget::GetMinimap() { return &m_minimap; }