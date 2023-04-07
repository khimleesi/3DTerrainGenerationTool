#pragma once

/*******************************************************************************************************************
	Entity.h, Entity.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	An entity derives from base class Game Object. It is a game object with a model and a material.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "GameObject.h"
#include "graphics/Material.h"
#include "graphics/Model.h"
#include "physics/AABounds3D.h"

class Entity : public GameObject {

public:
	Entity(const std::string& tag, const Transform& transform, const Material& material, const Model& model, bool hasCollisionResponse);
	virtual ~Entity();

public:
	virtual void Update()				override;
	virtual void Render(Shader* shader) override;

public:
	static Entity* Create(const std::string& tag);

public:
	Material*			GetMaterial();
	Model*				GetModel();
	const AABounds3D&	GetBound() const;

public:
	bool HasCollisionResponse();

private:
	Material	m_material;
	Model		m_model;
	AABounds3D	m_bound;
	bool		m_hasCollisionResponse;
};
