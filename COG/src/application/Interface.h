#pragma once

/*******************************************************************************************************************
	Interface.h, Interface.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	Abstract base class for all 2D interface objects.
	
	[Features]
	Tagging system - every single interface object in the scene has a tag/id, which we can use to keep track of interface
	objects'. This is useful for things like triggered events, special collisions and handling correct destruction
	of memory.

	[Upcoming]
	Nothing.

	[Side Notes]
	The interface class is pretty much a clone of the 3D game objects' class.
	However, I wanted to keep 2D and 3D objects' seperate - they both use different shader's
	and both are rendered differently and so this allows me to see what's what easily.

*******************************************************************************************************************/
#include <string>
#include "physics/Transform.h"
#include "graphics/shaders/Shader.h"

class Interface {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_tag),
				COG_NVP(m_transform));
	}

public:
	Interface(const std::string& tag, const Transform& transform);
	Interface();
	virtual ~Interface() = 0;

public:
	virtual void Render(Shader* shader) = 0;
	virtual void Update() = 0;

public:
	bool IsActive() const;
	bool IsAlive() const;

public:
	void SetActive(bool isActive);
	void SetAlive(bool isAlive);

public:
	Transform* GetTransform();
	const std::string& GetTag() const;

protected:
	std::string m_tag;
	Transform	m_transform;

protected:
	bool m_isAlive;
	bool m_isActive;
};