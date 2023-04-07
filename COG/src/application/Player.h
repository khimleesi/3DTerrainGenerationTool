#pragma once
/*******************************************************************************************************************
	Payer.h, Payer.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	The main player class. Derives from Game Object.

	[Features]
	Nothing fancy.

	[Upcoming]
	Better AI/Movement. Swept AABB collision for nice wall sliding, plus crouch/sprint, etc.
	Controller movement.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "GameObject.h"
#include "physics/AABounds3D.h"
#include "application/Terrain.h"
#include "application/Inventory.h"
#include "graphics/Light.h"

class Player : public GameObject {

public:
	Player(const std::string& tag, const Transform& transform);
	~Player();

public:
	virtual void Update() override;
	virtual void Render(Shader* shader) override;

public:
	static Player* Create(const std::string& tag);
	
public:
	void SetGround(Terrain* terrain);
	void SetFlashlight(Light* flashlight);
	void Stop();
	void PickUp(GameObject* object);

public:
	const AABounds3D& GetBound() const;
	bool HasCollectedAllItems() const;

private:
	void ProcessInput();
	void FollowTerrain();
	void ConstrainBounds();
	void UpdateInventory();
	void UpdateFlashlight();

private:
	void MoveForward();
	void MoveBack();
	void RotateLeft();
	void RotateRight();

private:
	bool	m_displayInventory;
	bool	m_walking;

private:
	glm::vec3	m_previousPosition;
	Terrain*	m_terrain;
	Light*		m_flashLight;
	AABounds3D	m_bound;
	Inventory	m_inventory;

private:
	static const float s_offsetFromGround;
	static const float s_defaultMovementSpeed;
	static const float s_defaultRotationSpeed;
};