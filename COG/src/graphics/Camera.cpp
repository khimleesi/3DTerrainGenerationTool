#include <pretty_glm/gtx/rotate_vector.hpp>
#include "Camera.h"
#include "managers/ScreenManager.h"
#include "managers/GameManager.h"
#include "managers/InputManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Camera::Camera(const std::string& tag, const glm::vec3& position, const glm::vec3& rotation)
	:	GameComponent(tag),
		m_position(position),
		m_rotation(glm::radians(rotation)),
		m_forward(s_defaultForwardAxis),
		m_up(s_defaultUpAxis),
		m_right(s_defaultRightAxis),
		m_viewMatrix(1.0f),
		m_rotationMatrix(1.0f),
		m_translationMatrix(1.0f),
		m_isDirty(true)
{
	//--- Update the cameras view matrix when an instance is first created
	Update();
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
Camera::~Camera()
{

}


/*******************************************************************************************************************
	Function that moves the camera, taking in the direction we want to move and multiplying that by some amount
*******************************************************************************************************************/
void Camera::Move(const glm::vec3& direction, float amount)
{
	m_position += direction * amount * Game::Instance()->GetDeltaTime();

	m_isDirty = true;
}


/*******************************************************************************************************************
	Function that adjusts the FOV angle of the perspective matrix to immitate a camera zoom in/out feature
*******************************************************************************************************************/
void Camera::Zoom(float distance)
{
	float currentFieldOfView = Screen::Instance()->GetFieldOfView();
	float newFieldOfView = currentFieldOfView + glm::radians(distance) * Game::Instance()->GetDeltaTime();

	//--- Only update the field of view when a change has happened
	if (currentFieldOfView != newFieldOfView) {

		using namespace screen_constants;
		//--- And constrain our new FOV to our min/max zoom in/out values
		if (newFieldOfView < MIN_FIELD_OF_VIEW) { newFieldOfView = MIN_FIELD_OF_VIEW; }
		if (newFieldOfView > MAX_FIELD_OF_VIEW) { newFieldOfView = MAX_FIELD_OF_VIEW; }

		Screen::Instance()->SetFieldOfView(newFieldOfView - currentFieldOfView);
	}
}


/*******************************************************************************************************************
	Function that rotates the camera using euler angles
*******************************************************************************************************************/
void Camera::Rotate(float pitch, float yaw, float roll)
{
	m_rotation.x += glm::radians(pitch) * Game::Instance()->GetDeltaTime();
	m_rotation.y += glm::radians(yaw) * Game::Instance()->GetDeltaTime();
	m_rotation.z += glm::radians(roll) * Game::Instance()->GetDeltaTime();

	//--- Constrain the pitch and yaw rotations to our min/max rotation values
	if (m_rotation.x >  s_maxRotation.x) { m_rotation.x =  s_maxRotation.x; }
	if (m_rotation.x < -s_maxRotation.x) { m_rotation.x = -s_maxRotation.x; }
	//if (m_rotation.y >  s_maxRotation.y) { m_rotation.y =  s_maxRotation.y; }
	//if (m_rotation.y < -s_maxRotation.y) { m_rotation.y = -s_maxRotation.y; }

	m_isDirty = true;
}


/*******************************************************************************************************************
	Called every frame, checks if any matrices have changed and updates the overall view matrix if so
*******************************************************************************************************************/
void Camera::Update()
{
	//--- If the camera is parented to a Game Object, its transformations will be done relative to that object
	if (HasParent()) {
		glm::vec3 parentPosition = GetParentTransform()->GetPosition();
		glm::vec3 parentRotation = GetParentTransform()->GetRotation();

		if (m_position != parentPosition) { m_position = parentPosition; m_isDirty = true; }
		if (m_rotation != parentRotation) { m_rotation = parentRotation; m_isDirty = true; }
	}
	
	if (m_isDirty) {
		UpdateRotationMatrix();
		UpdateTranslationMatrix();
		UpdateViewMatrix();
		m_isDirty = false;
	}
}


/*******************************************************************************************************************
	Function that updates only the rotation matrix of the camera
*******************************************************************************************************************/
void Camera::UpdateRotationMatrix()
{
	m_rotationMatrix =	glm::rotate(m_rotation.z, glm::vec3(s_defaultForwardAxis)) *
						glm::rotate(m_rotation.x, glm::vec3(s_defaultRightAxis)) *
						glm::rotate(m_rotation.y, glm::vec3(s_defaultUpAxis));
}


/*******************************************************************************************************************
	Function that updates only the translation matrix of the camera
*******************************************************************************************************************/
void Camera::UpdateTranslationMatrix() {

	m_translationMatrix = glm::translate(-m_position);
}


/*******************************************************************************************************************
	Updates the overall view matrix & direction vectors - only calculated when changes have been applied
*******************************************************************************************************************/
void Camera::UpdateViewMatrix()
{
	m_viewMatrix = m_rotationMatrix * m_translationMatrix;

	//--- Retrieve the directional data directly from the view matrix, rather than performing extra calculations
	m_forward	= glm::normalize(glm::vec3(-m_viewMatrix[0][2], -m_viewMatrix[1][2], -m_viewMatrix[2][2]));
	m_up		= glm::normalize(glm::vec3(m_viewMatrix[0][1], m_viewMatrix[1][1], m_viewMatrix[2][1]));
	m_right		= glm::normalize(glm::vec3(m_viewMatrix[0][0], m_viewMatrix[1][0], m_viewMatrix[2][0]));
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
glm::vec3 Camera::GetPosition() const			{ return m_position; }
glm::vec3 Camera::GetRotation() const			{ return m_rotation; }

glm::vec3 Camera::GetRight() const				{ return m_right; }
glm::vec3 Camera::GetForward() const			{ return m_forward; }
glm::vec3 Camera::GetUp() const					{ return m_up; }

glm::mat4 Camera::GetViewMatrix() const			{ return m_viewMatrix; }
glm::mat4 Camera::GetRotationMatrix() const		{ return m_rotationMatrix; }
glm::mat4 Camera::GetTranslationMatrix() const	{ return m_translationMatrix; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Camera::SetPosition(const glm::vec3& position)	{ m_position = position; m_isDirty = true; }
void Camera::SetRotation(const glm::vec3& rotation)	{ m_rotation = glm::radians(rotation); m_isDirty = true; }

void Camera::SetForward(const glm::vec3& forward)	{ m_forward = forward; m_isDirty = true; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
glm::vec3 Camera::s_defaultUpAxis		= glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Camera::s_defaultForwardAxis	= glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::s_defaultRightAxis	= glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec2 Camera::s_maxRotation			= glm::vec2(0.4f, 1.0f);