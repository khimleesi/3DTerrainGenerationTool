#pragma once

/*******************************************************************************************************************
	Picker.h, Picker.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	A picker class, which is used to cast a directional ray from a 2D point(x, y)
	to a 3D position in world space.

	[Features]
	Supports mouse picking.
	Only checks for collision's when object's are within range.

	[Upcoming]
	Support for PS4 controller.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include "graphics/Camera.h"
#include "physics/AABounds3D.h"

class Picker {

public:
	Picker(Camera* camera);
	~Picker();

public:
	void Update();

public:
	const glm::vec3& GetRay() const;

public:
	bool IsColliding(const AABounds3D& bounds, float range);

private:
	glm::vec3 CalculateMouseRay();
	glm::vec2 GetNormalizedDeviceCoordinates(const glm::vec2& mousePosition);
	glm::vec4 GetEyeSpaceCoordinates(const glm::vec4& clipCoordinates);
	glm::vec3 GetWorldCoordinates(const glm::vec4& eyeCoordinates);

private:
	glm::vec3 m_ray;
	glm::vec3 m_origin;
	glm::vec3 m_margin;
	glm::vec3 m_direction;
	
private:
	Camera*		m_camera;
};