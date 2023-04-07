#pragma once

/*******************************************************************************************************************
	Skybox.h, Skybox.cpp
	Created by Kim Kane
	Last updated: 21/03/2018
	Class finalized: 02/04/2018

	Generates a skybox of any size around our 3D world.

	[Features]
	Supports OpenGL cube maps.
	Size can be changed dynamically.

	[Upcoming]
	Skydomes.
	Animated skyboxes.

	[Side Notes]
	The skybox doesn't have a transform as we always want it to encase our entire world.
	All it really is, is a cube primitive with a texture applied.
	You can adjust the size of the skybox to scale it, but skyboxes generally are positioned around the world.

*******************************************************************************************************************/
#include <string>
#include "graphics/Texture.h"
#include "application/Cube.h"

class Shader;

class Skybox {

public:
	Skybox( const std::string& tag,
			const std::string& left,
			const std::string& right,
			const std::string& top,
			const std::string& bottom,
			const std::string& front,
			const std::string& back,
			float size = 50.0f);
public:
	~Skybox();

public:
	void Render(Shader* shader);

private:
	void Load();

private:
	std::string		m_tag;
	float			m_size;
	Texture			m_texture;
	Cube			m_cube;
};