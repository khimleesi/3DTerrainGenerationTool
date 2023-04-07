#include "Inventory.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"
#include "managers/InputManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Inventory::Inventory(const std::string& tag, const std::string& texture, const Transform& transform)
	:	Interface(tag + ".inventory", transform),
		m_sprite(m_tag, texture)
{
	//--- Reserve memory for max items when an instance of the inventory is created
	m_items.reserve(s_maxItems);
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Inventory::~Inventory()
{
	for (auto item : m_items) { delete item; item = nullptr; }
}


/*******************************************************************************************************************
	A function that updates the inventory and its contained items
*******************************************************************************************************************/
void Inventory::Update()
{
	m_transform.Update();

	for (auto item : m_items) { item->Update(); Display(item); }
}


/*******************************************************************************************************************
	A function that renders the inventory and its contained items
*******************************************************************************************************************/
void Inventory::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		interfaceShader->SetInstanceData(&m_transform, m_sprite.GetTexture());

		m_sprite.Render();

		for (auto item : m_items) { item->Render(shader); }
	}
}


/*******************************************************************************************************************
	A function that finds an object within the inventory (not currently being used)
*******************************************************************************************************************/
InventoryItem* Inventory::Find(const std::string& tag)
{
	for (auto item : m_items) { if (item->GetTag() == tag) { return item; } }

	return nullptr;
}


/*******************************************************************************************************************
	A function that adds a new object to the inventory
*******************************************************************************************************************/
void Inventory::Add(const std::string& tag)
{
	glm::vec2 offset = glm::vec2(0.0f);

	if (!m_items.empty()) {

		//--- Set the previous item as disactive, incase player still has it open upon pickup of this new item
		m_items.back()->SetActive(false);

		//--- Get the position of the last item added
		glm::vec2 previousItemPosition = m_items.back()->GetIcon()->transform.GetPosition();

		//--- Offset this items position by previous item position plus divider amount
		offset.x = (previousItemPosition.x + s_defaultDivider.x);
		
		//--- If the offset is greater than the inventory width then move item down; decrease Y offset
		if (offset.x >= s_maxDimension) {
			offset.x = s_defaultOffset.x;
			offset.y = (previousItemPosition.y - s_defaultDivider.y);
		}
		//--- Otherwise Y offset remains the same, but still increment X
		else { offset.y = previousItemPosition.y; }
	}

	//--- If the inventory is empty, set the first items initial position
	else { 
		offset.x = s_defaultOffset.x;
		offset.y = s_defaultOffset.y;
	}

	std::string texture = "Inventory\\Icons\\" + tag + ".png";

	//--- Create a new item
	InventoryItem* item = new InventoryItem(tag, texture, Transform(glm::vec2(220, 220), glm::vec2(350.0f, 350.0f)));
	
	//--- Set the position of this items icon to the offsets calculated above
	item->GetIcon()->transform.SetPosition(offset.x, offset.y);

	//--- Add item to the inventory
	m_items.emplace_back(item);
}


/*******************************************************************************************************************
	A function that display's an object within the inventory
*******************************************************************************************************************/
void Inventory::Display(InventoryItem* item)
{
	//--- If the item is not currently active (only the icon is displayed)
	if (!item->IsActive()) {
		//--- Check if we have a collision with the items icon
		if (Input::Instance()->IsMouseColliding(item->GetIcon()->bound)) {

			if (Input::Instance()->IsMouseButtonPressed(SDL_BUTTON_LEFT, false)) {
				//--- If we do, make sure we turn off any previous items (avoid items being drawn on top of one-another)
				for (auto other : m_items) { if (other->IsActive()) { other->SetActive(false); } }
			
				//--- Make the item active, now we want to display it
				item->SetActive(true);
			}
		}
	}
	else {
		//--- If the item is active we check for a collision between the mouse and the items bounding box instead
		if (Input::Instance()->IsMouseColliding(*item->GetBound())) {

			if (Input::Instance()->IsMouseButtonPressed(SDL_BUTTON_LEFT, false)) {
				item->SetActive(false);
			}
		}
	}

	//--- NOTE
	// Upon closing and re-opening the inventory, the item will stay active.
	// I designed it this way, as I wanted the player to be able to toggle the inventory on/off quickly,
	// and not lose the item they may have been focusing on, having to re-click or find it again
	// However this can be changed to something more generic in future.
	// Worth a mention in case someone thinks it's a bug.
	//---
}


/*******************************************************************************************************************
	Function which checks if all of the items have been collected
*******************************************************************************************************************/
bool Inventory::IsInventoryFull() const { return m_items.size() == s_maxItems; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const glm::vec2 Inventory::s_defaultOffset	= glm::vec2(7.0f, 150.0f);
const glm::vec2 Inventory::s_defaultDivider = glm::vec2(64.0f, 78.0f);
const float Inventory::s_maxDimension		= 256.0f;
const unsigned int Inventory::s_maxItems	= 8;