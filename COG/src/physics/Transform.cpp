#include <pretty_glm/gtx/rotate_vector.hpp>
#include "Transform.h"
#include "managers/GameManager.h"
#include "utilities/Log.h"
#include <math.h>

/*******************************************************************************************************************
	[3D] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	:	m_position(position),
		m_rotation(glm::radians(rotation)),
		m_scale(scale),
		m_translationMatrix(1.0f),
		m_rotationMatrix(1.0f),
		m_scaleMatrix(1.0f),
		m_transformationMatrix(1.0f),
		m_isDirty(true)
{
	//--- Update the transformation matrix when an instance is first created
	Update();
}


/*******************************************************************************************************************
	[2D] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Transform::Transform(const glm::vec2& position, const glm::vec2& dimension)
	:	m_position(position.x, position.y, s_defaultForwardAxis.z),
		m_rotation(0.0f),
		m_scale(dimension.x, dimension.y, 1.0f),
		m_translationMatrix(1.0f),
		m_rotationMatrix(1.0f),
		m_scaleMatrix(1.0f),
		m_transformationMatrix(1.0f),
		m_isDirty(true)
{
	//--- Update the transformation matrix when an instance is first created
	Update();
}


Transform::Transform()
	:	m_position(0.0f, 0.0f, s_defaultForwardAxis.z),
		m_rotation(0.0f),
		m_scale(1.0f),
		m_translationMatrix(1.0f),
		m_rotationMatrix(1.0f),
		m_scaleMatrix(1.0f),
		m_transformationMatrix(1.0f),
		m_isDirty(true)
{
	Update();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
Transform::~Transform()
{

}


/*******************************************************************************************************************
	Function that moves an object, taking in the direction we want to move and multiplying that by some amount
*******************************************************************************************************************/
void Transform::Move(const glm::vec3& direction, float amount)
{
	m_position += direction * amount * Game::Instance()->GetDeltaTime();
	
	m_isDirty = true;
}


/*******************************************************************************************************************
	Function that rotates an object using euler angles
*******************************************************************************************************************/
void Transform::Rotate(float pitch, float yaw, float roll)
{
	m_rotation.x += glm::radians(pitch) * Game::Instance()->GetDeltaTime();
	m_rotation.y += glm::radians(yaw) * Game::Instance()->GetDeltaTime();
	m_rotation.z += glm::radians(roll) * Game::Instance()->GetDeltaTime();
	
	//--- Constrain the pitch and yaw rotations to our min/max rotation values
	if (m_rotation.x >  s_maxRotation.x) { m_rotation.x = s_maxRotation.x; }
	if (m_rotation.x < -s_maxRotation.x) { m_rotation.x = -s_maxRotation.x; }
	//if (m_rotation.y >  s_maxRotation.y) { m_rotation.y =  s_maxRotation.y; }
	//if (m_rotation.y < -s_maxRotation.y) { m_rotation.y = -s_maxRotation.y; }

	m_isDirty = true;
}


/*******************************************************************************************************************
	Called every frame, checks if any matrices have changed and updates the overall world matrix if so
*******************************************************************************************************************/
void Transform::Update()
{
	if (m_isDirty) {

		UpdateTranslationMatrix();
		UpdateRotationMatrix();
		UpdateScaleMatrix();
		UpdateTransformationMatrix();

		m_isDirty = false;
	}
}


/*******************************************************************************************************************
	Function that updates only the translation matrix of an object
*******************************************************************************************************************/
void Transform::UpdateTranslationMatrix()
{
	m_translationMatrix = glm::translate(m_position);
}


/*******************************************************************************************************************
	Function that updates only the rotation matrix of an object
*******************************************************************************************************************/
void Transform::UpdateRotationMatrix()
{
	m_rotationMatrix =	glm::rotate(m_rotation.z, glm::vec3(s_defaultForwardAxis)) *
						glm::rotate(m_rotation.x, glm::vec3(s_defaultRightAxis)) *
						glm::rotate(m_rotation.y, glm::vec3(s_defaultUpAxis));
}


/*******************************************************************************************************************
	Function that updates only the scale matrix of an object
*******************************************************************************************************************/
void Transform::UpdateScaleMatrix()
{
	m_scaleMatrix = glm::scale(m_scale);
}


/*******************************************************************************************************************
	Updates the overall transformation matrix of an object - only calculated when changes have been applied
*******************************************************************************************************************/
void Transform::UpdateTransformationMatrix()
{
	m_transformationMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;

	//--- Retrieve the directional data directly from the transformation matrix, rather than performing extra calculations
	m_forward	= glm::normalize(glm::vec3(-m_transformationMatrix[0][2], -m_transformationMatrix[1][2], -m_transformationMatrix[2][2]));
	m_up		= glm::normalize(glm::vec3(m_transformationMatrix[0][1], m_transformationMatrix[1][1], m_transformationMatrix[2][1]));
	m_right		= glm::normalize(glm::vec3(m_transformationMatrix[0][0], m_transformationMatrix[1][0], m_transformationMatrix[2][0]));
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec3& Transform::GetPosition() const	{ return m_position; }
const glm::vec3& Transform::GetRotation() const	{ return m_rotation; }
const glm::vec3& Transform::GetScale() const	{ return m_scale; }
glm::vec2 Transform::GetDimensions() const		{ return glm::vec2(m_scale.x, m_scale.y); }

glm::vec3 Transform::GetRight() const { return m_right; }
glm::vec3 Transform::GetForward() const { return m_forward; }
glm::vec3 Transform::GetUp() const { return m_up; }

const glm::mat4& Transform::GetTransformationMatrix() const	{ return m_transformationMatrix; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Transform::SetScale(const glm::vec3& scale)		{ m_scale = scale; m_isDirty = true; }
void Transform::SetPosition(const glm::vec3& position)	{ m_position = position; m_isDirty = true; }
void Transform::SetRotation(const glm::vec3& rotation)	{ m_rotation = glm::radians(rotation); m_isDirty = true; }

void Transform::SetDimensions(float width, float height)	{ m_scale = glm::vec3(width, height, 1.0f); m_isDirty = true; }
void Transform::SetPosition(float x, float y)				{ m_position = glm::vec3(x, y, s_defaultForwardAxis.z); m_isDirty = true; }

void Transform::SetX(float x) { m_position.x = x; m_isDirty = true; }
void Transform::SetY(float y) { m_position.y = y; m_isDirty = true; }
void Transform::SetZ(float z) { m_position.z = z; m_isDirty = true; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
glm::vec3 Transform::s_defaultUpAxis		= glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Transform::s_defaultForwardAxis	= glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Transform::s_defaultRightAxis		= glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec2 Transform::s_maxRotation			= glm::vec2(0.4f, 1.0f);


/*******************************************************************************************************************
	Operator overload functions
*******************************************************************************************************************/
bool Transform::operator==(const Transform& other) const
{
	return	m_position	== other.m_position &&
			m_rotation	== other.m_rotation &&
			m_scale		== other.m_scale;
}

Transform Transform::operator+(const Transform & other) const
{
	return Transform(m_position + other.m_position, m_rotation + other.m_rotation);
}
