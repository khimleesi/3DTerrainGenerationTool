#pragma once
/*******************************************************************************************************************
	MinimapWidget.h, MinimapWidget.cpp
	Created by Kim Kane
	Last updated: 29/04/2018

	Derives from Interface. Creates an interface widget to surround the minimap.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "Interface.h"
#include "Sprite.h"
#include "Button.h"

class Widget : public Interface {

public:
	Widget(const std::string& tag, const std::string& texture, const Transform& transform, const glm::vec2& togglePosition);
	~Widget();

public:
	virtual void Update()				override;
	virtual void Render(Shader* shader) override;

public:
	static Widget* Create(const std::string& tag);

private:
	void ProcessInput();

private:
	Sprite	m_sprite;
	Button	m_close;
	Button	m_open;

	static const float s_defaultOffset;
	static const float s_defaultCloseDimensions;
	static const float s_defaultOpenDimensions;
};
