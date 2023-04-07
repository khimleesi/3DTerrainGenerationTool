#pragma once
/*******************************************************************************************************************
	SamplePlayer.h, SamplePlayer.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	The sample player class. Derives from Game Object.

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

class SamplePlayer : public GameObject {

public:
	SamplePlayer(const std::string& tag, const Transform& transform);
	~SamplePlayer();

public:
	virtual void Update() override;
	virtual void Render(Shader* shader) override;

public:
	static SamplePlayer* Create(const std::string& tag);

public:
	void SetGround(Terrain* terrain);
	void Stop();


public:
	const AABounds3D& GetBound() const;

private:
	void ProcessInput();
	void FollowTerrain();
	void ConstrainBounds();

private:
	void MoveForward();
	void MoveBack();
	void RotateLeft();
	void RotateRight();

private:
	bool	m_walking;

private:
	glm::vec3	m_previousPosition;
	Terrain*	m_terrain;
	AABounds3D	m_bound;

private:
	static const float s_offsetFromGround;
	static const float s_defaultMovementSpeed;
	static const float s_defaultRotationSpeed;
};
