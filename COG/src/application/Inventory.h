#pragma once
/*******************************************************************************************************************
	Inventory.h, Inventory.cpp
	Created by Kim Kane
	Last updated: 28/04/2018

	A derived class of base class Interface. A basic inventory, which can be used in conjunction with a
	Player object or as a standalone object. Currently not generic, as only setup for this specific game in mind.
	
	[Features]
	Nothing fancy.

	[Upcoming]
	A more advanced and generic inventory system.

	[Side Notes]
	Nothing.

*******************************************************************************************************************/
#include <vector>
#include "InventoryItem.h"
#include "Interface.h"
#include "Sprite.h"

class Inventory : public Interface {

public:
	Inventory(const std::string& tag, const std::string& texture, const Transform& transform);
	~Inventory();

public:
	virtual void Update() override;
	virtual void Render(Shader* shader) override;

public:
	void Add(const std::string& tag);
	bool IsInventoryFull() const;

private:
	void Display(InventoryItem* item);
	InventoryItem* Find(const std::string& tag);

private:
	Sprite m_sprite;

private:
	std::vector<InventoryItem*>	m_items;

private:
	static const glm::vec2		s_defaultOffset;
	static const glm::vec2		s_defaultDivider;
	static const float			s_maxDimension;
	static const unsigned int	s_maxItems;
};