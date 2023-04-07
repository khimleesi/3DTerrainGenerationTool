#include <algorithm>
#include "Picker.h"
#include "managers/InputManager.h"
#include "managers/ScreenManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Picker::Picker(Camera* camera)
	:	m_ray(0.0f),
		m_origin(0.0f),
		m_margin(0.0f),
		m_direction(0.0f),
		m_camera(camera)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Picker::~Picker()
{
	if (m_camera) { m_camera = nullptr; }
}


/*******************************************************************************************************************
	A function which updates the 3D ray every frame
*******************************************************************************************************************/
void Picker::Update()
{
	//--- Get the ray and the camera's position
	glm::vec3 currentRay	= CalculateMouseRay();
	glm::vec3 currentOrigin	= m_camera->GetPosition();

	//--- If any changes have happened to the ray or the camera position, update the new values
	if (m_ray != currentRay || m_origin != currentOrigin) {

		m_ray		= currentRay;
		m_origin	= currentOrigin;

		m_margin	= (m_origin + m_ray);
		m_direction = (m_margin - m_origin);
	}
}


/*******************************************************************************************************************
	A function which checks if the 3D ray has collided with a 3D AABB
	References:
	https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
	http://www.realtimerendering.com/intersections.html
	https://www.youtube.com/watch?v=fQOeEA_8-uk
*******************************************************************************************************************/
bool Picker::IsColliding(const AABounds3D& bounds, float range)
{
	//--- Get an average distance to the origin based on the bounds min/max values
	float distanceToOrigin = glm::distance(m_origin, (bounds.GetMin() + bounds.GetMax()) / 2.0f);

	//--- We only perform collision checks if we are within range of the object
	if (distanceToOrigin > range) { return false; }
	
	//--- Inverse the direction so we can multiply instead of divide
	glm::vec3 direction	= (1.0f / m_direction);
	glm::vec3 minimum	= glm::vec3(0.0f);
	glm::vec3 maximum	= glm::vec3(0.0f);

	//--- Get tminX/tmaxX [multiplied] by the [inverse direction] for optimization
	minimum.x = (bounds.GetMin().x - m_origin.x) * direction.x;
	maximum.x = (bounds.GetMax().x - m_origin.x) * direction.x;
	
	//--- If tmin is greater than tmax, swap them
	if (minimum.x > maximum.x) { std::swap(minimum.x, maximum.x); }

	//--- Do the same thing for the Y
	minimum.y = (bounds.GetMin().y - m_origin.y) * direction.y;
	maximum.y = (bounds.GetMax().y - m_origin.y) * direction.y;

	if (minimum.y > maximum.y) { std::swap(minimum.y, maximum.y); }

	//--- No collision
	if ((minimum.x > maximum.y) || (minimum.y > maximum.x)) { return false; }
	
	//--- Get the greatest minimum value
	if (minimum.y > minimum.x) { minimum.x = minimum.y; }
	
	//--- Get the smallest maximum value
	if (maximum.y < maximum.x) { maximum.x = maximum.y; }

	//--- Compute the tminZ/tmaxZ
	minimum.z = (bounds.GetMin().z - m_origin.z) * direction.z;
	maximum.z = (bounds.GetMax().z - m_origin.z) * direction.z;

	if (minimum.z > maximum.z) { std::swap(minimum.z, maximum.z); };

	//--- No collision
	if ((minimum.x > maximum.z) || (minimum.z > maximum.x)) { return false; }

	//--- Create a temporary bound for the origin of the ray and return false if the origin is inside the AABB
	AABounds3D originBound(m_origin, glm::vec3(0.1f), glm::vec3(1.0f), true);

	if (originBound.IsColliding(bounds)) { return false; }
	
	//--- If the maximum values is less than or equal to 0
	//--- The ray (line) is intersecting AABB, but the whole AABB is behind us, so return false
	if (maximum.x <= 0.0f || maximum.y <= 0.0f || maximum.z <= 0.0f) { return false; }

	//--- If we made it this far we have an intersection, so return true
	return true;
}


/*******************************************************************************************************************
	A function which calculates the 3D ray
	Reference: http://antongerdelan.net/opengl/raycasting.html
*******************************************************************************************************************/
glm::vec3 Picker::CalculateMouseRay() {

	//--- Get the 2D screen position of the ray we want to make
	glm::vec2 mousePosition	= Input::Instance()->GetMousePosition();

	//--- Convert this 2D position from screen coords into OpenGL coords (-1, 1)
	glm::vec2 normalized = GetNormalizedDeviceCoordinates(mousePosition);

	//--- Calculate homogeneous clip space (make the directional vector point into the screen)
	glm::vec4 clipCoordinates = glm::vec4(normalized.x, normalized.y, -1.0f, 1.0f);

	//--- Get the eye space coordinates
	glm::vec4 eyeCoordinates = GetEyeSpaceCoordinates(clipCoordinates);

	//--- Calculate the world ray by the inverse of the camera's view matrix
	glm::vec3 worldRay = GetWorldCoordinates(eyeCoordinates);

	return worldRay;
}


/*******************************************************************************************************************
	A function which converts a 2D screen position(x, y) to device coordinates (-1, 1)
*******************************************************************************************************************/
glm::vec2 Picker::GetNormalizedDeviceCoordinates(const glm::vec2& mousePosition)
{
	glm::vec2 normalized = glm::vec2(0.0f);

	//--- If mouse position is flipped, return x and -y instead.
	normalized.x = (2.0f * mousePosition.x) / Screen::Instance()->GetWidth() - 1.0f;
	normalized.y = 1.0f - mousePosition.y / Screen::Instance()->GetHeight() * 2.0f;

	return glm::vec2(normalized.x, normalized.y);
}


/*******************************************************************************************************************
	A function which get's the eye coordinates by taking the inverse of a projection matrix
*******************************************************************************************************************/
glm::vec4 Picker::GetEyeSpaceCoordinates(const glm::vec4& clipCoordinates)
{
	Screen::Instance()->PerspectiveView(true);

	glm::mat4 invertedProjection	= glm::inverse(Screen::Instance()->GetProjectionMatrix());
	glm::vec4 eyeCoordinates		= invertedProjection * clipCoordinates;

	return glm::vec4(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
}


/*******************************************************************************************************************
	A function which get's the world coordinates by taking the inverse of a view matrix
*******************************************************************************************************************/
glm::vec3 Picker::GetWorldCoordinates(const glm::vec4& eyeCoordinates)
{
	glm::mat4 invertedView	= glm::inverse(m_camera->GetViewMatrix());
	glm::vec4 worldRay		= invertedView * eyeCoordinates;
	glm::vec3 mouseRay		= glm::vec3(worldRay.x, worldRay.y, worldRay.z);

	glm::normalize(mouseRay);

	return mouseRay;
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec3& Picker::GetRay() const	{ return m_ray; }