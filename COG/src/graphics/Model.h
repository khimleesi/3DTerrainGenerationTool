#pragma once

/*******************************************************************************************************************
	Model.h, Model.cpp
	Created by Kim Kane
	Last updated: 21/03/2018
	Class finalized: 21/03/2018

	Generates a 3D model from an OBJ file and fills the OpenGL buffers.

	[Features]
	Supports Assimp model loading.
	Models only get created once - all models with the same name will re-use models already loaded.
	(See ResourceManager to see how this works)
	Supports obtaining the min/max extents of a model; able to retrieve width, height and depth
	of model by calling the GetDimension function.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	None at present.

*******************************************************************************************************************/
#include <map>
#include <string>
#include "graphics/buffers/VertexBuffer.h"

class Model {

public:
	Model(const std::string& obj);
	~Model();

public:
	void Render();

public:
	const glm::vec3& GetDimension() const;

private:
	bool Load();
	void CalculateDimension(std::vector<VertexBuffer::PackedVertex>& container);

private:
	std::string	m_tag;

private:
	static std::map<std::string, glm::vec3> m_dimensions;
};