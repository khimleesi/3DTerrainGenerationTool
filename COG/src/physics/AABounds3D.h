#pragma once
/*******************************************************************************************************************
	AABounds2D.h, AABounds3D.cpp
	Created by Kim Kane
	Last updated: 17/04/2018

	Generates a simple 3D axis aligned bounding box to be used for collision between 3D objects.

	[Features]
	Only updates data if a change has happened, saving calculations.
	Supports 3D models - providing origin is in bottom left corner, or bottom centered.
	Supports disabling of bounding boxes at any time (call function Disable())

	[Upcoming]
	Nothing at present.

	[Side Notes]
	None at present.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>

class AABounds3D {

public:
	AABounds3D(const glm::vec3& position, const glm::vec3& dimension, const glm::vec3& scale, bool isCentered);
	~AABounds3D();

public:
	void Update(const glm::vec3& position, const glm::vec3& dimension, const glm::vec3& scale);
	bool IsColliding(const AABounds3D& other) const;
	void Disable();

public:
	const glm::vec3& GetDimension() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetMin() const;
	const glm::vec3& GetMax() const;
	const glm::vec3& GetHalfDimension() const;

private:
	void GenerateBounds();

private:
	AABounds3D(const AABounds3D&)				= delete;
	AABounds3D& operator=(const AABounds3D&)	= delete;

private:
	glm::vec3 m_min;
	glm::vec3 m_max;
	glm::vec3 m_position;
	glm::vec3 m_dimension;
	glm::vec3 m_scale;
	glm::vec3 m_halfDimension;

private:
	bool m_isCentered;
};