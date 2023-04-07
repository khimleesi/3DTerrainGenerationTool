#pragma once

/*******************************************************************************************************************
	Transform.h, Transform.cpp
	Created by Kim Kane
	Last updated: 09/03/2018

	A class created for all 2D and 3D game objects' orientations in a 3D world.

	[Features]
	Supports movement, rotation (using euler angles) and scaling of 2D and 3D objects.
	The objects' transformation matrix is only updated when changes have happened - using an optimization technique known
	as a dirty flag.

	[Upcoming]
	Support for quaternion rotation.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include "managers/FileManager.h"

class Transform {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_position),
				COG_NVP(m_rotation),
				COG_NVP(m_scale));
	}

public:
	Transform(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
	Transform(const glm::vec2& position, const glm::vec2& dimension);
	Transform();

	~Transform();

public:
	void Move(const glm::vec3& direction, float amount);
	void Rotate(float pitch, float yaw, float roll);
	void Update();

public:
	const glm::vec3&	GetPosition() const;
	const glm::vec3&	GetRotation() const;
	const glm::vec3&	GetScale() const;
	glm::vec2			GetDimensions() const;

public:
	const glm::mat4&	GetTransformationMatrix() const;

public:
	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);
	void SetDirty(bool isDirty) { m_isDirty = isDirty; }

public:
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);

public:
	void SetPosition(float x, float y);
	void SetDimensions(float width, float height);

public:
	glm::vec3 GetRight() const;
	glm::vec3 GetForward() const;
	glm::vec3 GetUp() const;

public:
	bool operator==(const Transform& other) const;
	Transform operator+(const Transform& other) const;

private:
	void UpdateTranslationMatrix();
	void UpdateRotationMatrix();
	void UpdateScaleMatrix();

private:
	void UpdateTransformationMatrix();

private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

private:
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;

private:
	glm::mat4 m_translationMatrix;
	glm::mat4 m_rotationMatrix;
	glm::mat4 m_scaleMatrix;

private:
	glm::mat4 m_transformationMatrix;

private:
	bool m_isDirty;

private:
	static glm::vec3 s_defaultUpAxis;
	static glm::vec3 s_defaultForwardAxis;
	static glm::vec3 s_defaultRightAxis;
	static glm::vec2 s_maxRotation;
};