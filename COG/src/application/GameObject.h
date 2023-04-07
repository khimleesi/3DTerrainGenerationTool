#pragma once

/*******************************************************************************************************************
	GameObject.h, GameObject.cpp
	Created by Kim Kane
	Last updated: 17/03/2018

	Abstract base class for all 3D game objects.
	
	[Features]
	Tagging system - every single game object in the scene has a tag/id, which we can use to keep track of game
	objects'. This is useful for things like triggered events, special collisions and handling correct destruction
	of memory.

	[Upcoming]
	Nothing.

	[Side Notes]
	The game objects' act as an "empty object" (similar to Unity). All instances are given
	nothing more than a tag/id and a 3D position in world space.

*******************************************************************************************************************/
#include <string>
#include <vector>

#include "managers/FileManager.h"
#include "physics/Transform.h"
#include "graphics/shaders/Shader.h"

class GameObject {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_tag),
				COG_NVP(m_transform));
	}

public:
	GameObject(const std::string& tag, const Transform& transform);
	GameObject();

	virtual ~GameObject() = 0;

public:
	virtual void Update() = 0;
	virtual void Render(Shader* shader) = 0;

public:
	Transform* GetTransform();
	const std::string& GetTag() const;
	void SetTag(const std::string& tag) { m_tag = tag; }

public:
	bool IsActive() const;
	bool IsAlive() const;

public:
	void SetActive(bool isActive);
	void SetAlive(bool isAlive);

private:
	GameObject(GameObject const&)				= delete;
	GameObject& operator=(GameObject const&)	= delete;

protected:
	std::string		m_tag;
	Transform		m_transform;

protected:
	bool			m_isAlive;
	bool			m_isActive;
};