#pragma once
/*******************************************************************************************************************
	InventoryItem.h, InventoryItem.cpp
	Created by Kim Kane
	Last updated: 29/04/2018

	Derives from Interface. A basic class which creates an inventory item, as well as an item icon (to display
	a smaller version of item in the inventory).

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "Interface.h"
#include "Sprite.h"
#include "physics/AABounds2D.h"

class InventoryItem : public Interface {

private:
	struct Icon {

		std::string tag;
		Sprite		icon;
		AABounds2D	bound;
		Transform	transform;

		Icon(const std::string& tag, const std::string& texture, glm::vec2 position, glm::vec2 dimension)
			: icon(tag + ".icon", texture), bound(position, dimension), transform(position, dimension) {}
	};

public:
	InventoryItem(const std::string& tag, const std::string& texture, const Transform& transform);
	~InventoryItem();

public:
	virtual void Update()				override;
	virtual void Render(Shader* shader) override;

public:
	Icon*		GetIcon();
	AABounds2D* GetBound();

private:
	Sprite		m_sprite;
	AABounds2D	m_bound;
	Icon		m_icon;
	
private:
	static const glm::vec2 s_defaultDimension;
};