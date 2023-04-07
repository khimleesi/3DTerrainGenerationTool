#include "InventoryItem.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
InventoryItem::InventoryItem(const std::string& tag, const std::string& texture, const Transform& transform)
	:	Interface(tag + ".item", transform),
		m_sprite(tag, texture),
		m_bound(transform.GetPosition(), transform.GetDimensions()),
		m_icon(tag, texture, glm::vec2(7.0f, 150.0f), glm::vec2(s_defaultDimension.x, s_defaultDimension.y))
	
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
InventoryItem::~InventoryItem()
{

}


/*******************************************************************************************************************
	A function that updates the inventory item (if active), along with its icon
*******************************************************************************************************************/
void InventoryItem::Update()
{
	m_icon.transform.Update();
	m_icon.bound.Update(m_icon.transform.GetPosition(), m_icon.transform.GetDimensions());

	if (m_isActive) {

		m_transform.Update();
		m_bound.Update(m_transform.GetPosition(), m_transform.GetDimensions());
	}
}


/*******************************************************************************************************************
	A function that renders the inventory item (if active), along with its icon
*******************************************************************************************************************/
void InventoryItem::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		interfaceShader->SetInstanceData(&m_icon.transform, m_icon.icon.GetTexture());
		m_icon.icon.Render();

		if (m_isActive) {
			interfaceShader->SetInstanceData(&m_transform, m_sprite.GetTexture());
			m_sprite.Render();
		}
	}
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
InventoryItem::Icon* InventoryItem::GetIcon()	{ return &m_icon; }
AABounds2D* InventoryItem::GetBound()			{ return &m_bound; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const glm::vec2 InventoryItem::s_defaultDimension = glm::vec2(50.0f, 64.0f);