#pragma once
/*******************************************************************************************************************
	AABounds2D.h, AABounds3D.cpp
	Created by Kim Kane
	Last updated: 17/04/2018

	Generates a simple 2D axis aligned bounding box to be used for collision between 2D objects.

	[Features]
	Only updates data if a change has happened, saving calculations.
	Supports disabling of bounding boxes at any time (call function Disable())

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>

class AABounds2D {

public:
	AABounds2D(const glm::vec2& position, const glm::vec2& dimension);
	~AABounds2D();

public:
	void Update(const glm::vec2& position, const glm::vec2& dimension);
	bool IsColliding(const AABounds2D& other) const;
	void Disable();

public:
	const glm::vec2& GetDimension() const;
	const glm::vec2& GetPosition() const;

private:
	void GenerateBounds();

private:
	AABounds2D(const AABounds2D&)				= delete;
	AABounds2D& operator=(const AABounds2D&)	= delete;

private:
	glm::vec2 m_min;
	glm::vec2 m_max;
	glm::vec2 m_position;
	glm::vec2 m_dimension;
};