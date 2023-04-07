#pragma once

/*******************************************************************************************************************
	Camera.h, Camera.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	A generic camera, which can be used as a 2D camera, FPS or 3PS. The camera is a Game Component, allowing
	you to give it a parented Game Object. This means all updates of the camera will be done in relation to the
	parented Game Object. Alternatively, you can also use it as a standalone object in the scene.

	[Features]
	Supports zoom in/out, movement and rotation (using euler angles).
	View matrix broken up into seperate matrices to allow us to grab different data (e.g. a skybox only needs the
	rotation matrix of the camera)
	The camera's view matrix is only updated when changes have happened - using an optimization technique known
	as a dirty flag. The zoom feature also only updates when a change has happened.

	[Upcoming]
	Support for quaternion rotation.

	[Side Notes]
	Note the camera doesn't make use of the 'Transform' class, it instead has a singular position and rotation variable.
	This is because the camera doesn't have a scale (or atleast, it doesn't make sense to give the camera
	a scale in this program, although I may change this later).

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include "application/GameComponent.h"

class Camera : public GameComponent {

public:
	Camera(const std::string& tag, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f));
	virtual ~Camera();

public:
	void Move(const glm::vec3& direction, float amount);
	void Rotate(float pitch, float yaw, float roll);
	void Zoom(float distance);

public:
	virtual void Update() override;

public:
	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;

public:
	glm::vec3 GetRight() const;
	glm::vec3 GetForward() const;
	glm::vec3 GetUp() const;

public:
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetRotationMatrix() const;
	glm::mat4 GetTranslationMatrix() const;

public:
	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);

public:
	void SetForward(const glm::vec3& forward);

private:
	void UpdateViewMatrix();
	void UpdateRotationMatrix();
	void UpdateTranslationMatrix();

private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;

private:
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;

private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_rotationMatrix;
	glm::mat4 m_translationMatrix;

private:
	bool m_isDirty;

private:
	static glm::vec3	s_defaultUpAxis;
	static glm::vec3	s_defaultForwardAxis;
	static glm::vec3	s_defaultRightAxis;
	static glm::vec2	s_maxRotation;
};