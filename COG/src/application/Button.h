#pragma once
/*******************************************************************************************************************
	Button.h, Button.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	A derived class of base class Interface. A button is pretty much what it says on the tin!
	
	[Features]
	Has a sprite contained within it, which allows for animated buttons easily.
	Has an additional hover sprite for when mouse hovers button.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing.

*******************************************************************************************************************/
#include "Interface.h"
#include "Sprite.h"
#include "physics/AABounds2D.h"

class Button : public Interface {

public:
	Button(const std::string& tag, const std::string& texture, const Transform& transform);
	virtual ~Button();

public:
	virtual void Render(Shader* shader) override;
	virtual void Update()				override;
	static Button* Create(const std::string& tag);

public:
	Sprite* GetSprite();
	const AABounds2D& GetBound() const;
	bool IsClicked(const std::string& tag);
	bool IsClicked();
	bool IsHovered() const;

private:
	void ProcessInput();

private:
	Sprite		m_idle;
	Sprite		m_hover;
	AABounds2D	m_bound;
	bool		m_isClicked;
	bool		m_isHovered;
};