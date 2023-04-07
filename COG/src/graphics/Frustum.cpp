#include "Frustum.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Frustum::Frustum(const glm::mat4& projection, const glm::mat4& view)
{
	//--- Prepare our frustum and reserve memory when an instance is first created
	m_frustum.resize(s_maxSides);
	for (unsigned int side = 0; side < s_maxSides; side++) {
		m_frustum[side].resize(s_maxPlaneContents);
	}

	//--- Update frustum when an instance is first created, to set the startup clipping planes
	Update(projection, view);
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Frustum::~Frustum()
{

}


/*******************************************************************************************************************
	A function which calculates a magnitude of a normal to a plane and then normalizes it
*******************************************************************************************************************/
void Frustum::NormalizePlane(std::vector <std::vector<float>>& frustum, SideType side) {
	
	//--- Here we calculate the magnitude of the normal to the plane (point A B C)
	//--- Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
	//--- To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = (float)sqrt(
		frustum[side][A] * frustum[side][A] +
		frustum[side][B] * frustum[side][B] +
		frustum[side][C] * frustum[side][C]
	);
	
	//--- Then we divide the plane's values by it's magnitude.
	//--- This makes it easier to work with.
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}


/*******************************************************************************************************************
	A function which creates 6 clipping planes from the projection and view matrices passed in
*******************************************************************************************************************/
void Frustum::Update(const glm::mat4& projection, const glm::mat4& view) {

	//--- This will hold the clipping planes (sides of the frustum)
	glm::mat4 clip = projection * view;

	//--- Now we actually want to get the sides of the frustum. To do this we take
	//--- the clipping planes we received above and extract the sides from them.

	//--- This will extract the RIGHT side of the frustum
	m_frustum[RIGHT][A] = clip[0][3] - clip[0][0];
	m_frustum[RIGHT][B] = clip[1][3] - clip[1][0];
	m_frustum[RIGHT][C] = clip[2][3] - clip[2][0];
	m_frustum[RIGHT][D] = clip[3][3] - clip[3][0];

	//--- Now that we have a normal (A, B, C) and a distance (D) to the plane,
	//--- we want to normalize that normal and distance.
	NormalizePlane(m_frustum, RIGHT);

	m_frustum[LEFT][A] = clip[0][3] + clip[0][0];
	m_frustum[LEFT][B] = clip[1][3] + clip[1][0];
	m_frustum[LEFT][C] = clip[2][3] + clip[2][0];
	m_frustum[LEFT][D] = clip[3][3] + clip[3][0];

	NormalizePlane(m_frustum, LEFT);

	m_frustum[BOTTOM][A] = clip[0][3] + clip[0][1];
	m_frustum[BOTTOM][B] = clip[1][3] + clip[1][1];
	m_frustum[BOTTOM][C] = clip[2][3] + clip[2][1];
	m_frustum[BOTTOM][D] = clip[3][3] + clip[3][1];

	NormalizePlane(m_frustum, BOTTOM);

	m_frustum[TOP][A] = clip[0][3] - clip[0][1];
	m_frustum[TOP][B] = clip[1][3] - clip[1][1];
	m_frustum[TOP][C] = clip[2][3] - clip[2][1];
	m_frustum[TOP][D] = clip[3][3] - clip[3][1];

	NormalizePlane(m_frustum, TOP);

	m_frustum[BACK][A] = clip[0][3] - clip[0][2];
	m_frustum[BACK][B] = clip[1][3] - clip[1][2];
	m_frustum[BACK][C] = clip[2][3] - clip[2][2];
	m_frustum[BACK][D] = clip[3][3] - clip[3][2];

	NormalizePlane(m_frustum, BACK);

	m_frustum[FRONT][A] = clip[0][3] + clip[0][2];
	m_frustum[FRONT][B] = clip[1][3] + clip[1][2];
	m_frustum[FRONT][C] = clip[2][3] + clip[2][2];
	m_frustum[FRONT][D] = clip[3][3] + clip[3][2];

	NormalizePlane(m_frustum, FRONT);
}


/*******************************************************************************************************************
	A function which checks if a single point is inside the frustum
*******************************************************************************************************************/
bool Frustum::IsPointInside(const glm::vec3& position) {

	//--- Go through all the sides of the frustum
	for (unsigned int side = 0; side < s_maxSides; side++) {
		//--- Calculate the plane equation and check if the point is behind a side of the frustum
		if (m_frustum[side][A] * position.x +
			m_frustum[side][B] * position.y +
			m_frustum[side][C] * position.z +
			m_frustum[side][D] <= 0.0f) {
			//--- The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	//--- The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


/*******************************************************************************************************************
	A function which checks if a sphere is inside the frustum, giving its center position and radius
*******************************************************************************************************************/
bool Frustum::IsSphereInside(const glm::vec3& centerPosition, float radius) {

	//--- Go through all the sides of the frustum
	for (unsigned int side = 0; side < s_maxSides; side++) {
		//--- If the center of the sphere is farther away from the plane than the radius
		if (m_frustum[side][A] * centerPosition.x +
			m_frustum[side][B] * centerPosition.y +
			m_frustum[side][C] * centerPosition.z +
			m_frustum[side][D] <= -radius) {
			//--- The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}

	//--- The sphere was inside of the frustum!
	return true;
}


/*******************************************************************************************************************
	A function which checks if a cube is inside the frustum given its center position and length / 2.0
*******************************************************************************************************************/
bool Frustum::IsCubeInside(const glm::vec3& centerPosition, float halfDepth) {
	
	//--- Extract the center position of the cube and half the length (depth(z axis) / 2.0)
	//--- Then we check each point in the cube and seeing if it is inside the frustum.
	//--- If a point is found in front of a side, then we skip to the next side.
	//--- If we get to a plane that does NOT have a point in front of it, then it will return false.

	for (unsigned int side = 0; side < s_maxSides; side++) {
		if (m_frustum[side][A] * (centerPosition.x - halfDepth) +
			m_frustum[side][B] * (centerPosition.y - halfDepth) +
			m_frustum[side][C] * (centerPosition.z - halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDepth) +
			m_frustum[side][B] * (centerPosition.y - halfDepth) +
			m_frustum[side][C] * (centerPosition.z - halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDepth) +
			m_frustum[side][B] * (centerPosition.y + halfDepth) +
			m_frustum[side][C] * (centerPosition.z - halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDepth) +
			m_frustum[side][B] * (centerPosition.y + halfDepth) +
			m_frustum[side][C] * (centerPosition.z - halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDepth) +
			m_frustum[side][B] * (centerPosition.y - halfDepth) +
			m_frustum[side][C] * (centerPosition.z + halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDepth) +
			m_frustum[side][B] * (centerPosition.y - halfDepth) +
			m_frustum[side][C] * (centerPosition.z + halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDepth) +
			m_frustum[side][B] * (centerPosition.y + halfDepth) +
			m_frustum[side][C] * (centerPosition.z + halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDepth) +
			m_frustum[side][B] * (centerPosition.y + halfDepth) +
			m_frustum[side][C] * (centerPosition.z + halfDepth) +
			m_frustum[side][D] >= 0.0f)
			continue;

		//--- If we get here, it isn't in the frustum
		return false;
	}

	return true;
}


/*******************************************************************************************************************
	A function which checks if a rectangle is inside the frustum given its center position half dimension
*******************************************************************************************************************/
bool Frustum::IsRectangleInside(const glm::vec3& centerPosition, const glm::vec3& halfDimension) {

	//--- Rectangles are the same as cubes only we take into acount the full dimension
	//--- Due to the sides being un-equal; so equivelant to width, height and depth of a models
	//--- bounding box / 2.0

	for (unsigned int side = 0; side < s_maxSides; side++) {
		if (m_frustum[side][A] * (centerPosition.x - halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y - halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z - halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y - halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z - halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y + halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z - halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y + halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z - halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y - halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z + halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y - halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z + halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x - halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y + halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z + halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		if (m_frustum[side][A] * (centerPosition.x + halfDimension.x) +
			m_frustum[side][B] * (centerPosition.y + halfDimension.y) +
			m_frustum[side][C] * (centerPosition.z + halfDimension.z) +
			m_frustum[side][D] >= 0.0f)
			continue;

		//--- If we get here, it isn't in the frustum
		return false;
	}

	return true;
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const unsigned int Frustum::s_maxPlaneContents	= 4;
const unsigned int Frustum::s_maxSides			= 6;