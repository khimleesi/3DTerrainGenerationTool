#pragma once

/*******************************************************************************************************************
	Text.h, Text.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	Renders text to the screen using FreeType font library.
	
	[Features]
	Supports FreeType font library.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	The in-game text is a standalone object - it doesn't relate to any other object in the game, due to
	its unique set up and the way it is rendered. It has its own shader and it is the last object drawn in any scene,
	as we want it to be on top of everything, including the 2D interface objects.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <string>
#include "physics/Transform.h"

class Shader;

class Text {

public:
	Text(const std::string& font, unsigned int size);
	~Text();

public:
	void Render(Shader* shader, const std::string& text, const Transform& transform, const glm::vec4& color);

public:
	const glm::vec4& GetColor() const;
	Transform* GetTransform();

private:
	bool Load(unsigned int size);
	void SetupBuffers();
	
private:
	std::string m_tag;
	Transform	m_transform;
	glm::vec4	m_color;

private:
	static const unsigned int s_numVertices;
};