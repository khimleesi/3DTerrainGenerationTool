#include "AABounds3D.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
AABounds3D::AABounds3D(const glm::vec3& position, const glm::vec3& dimension, const glm::vec3& scale, bool isCentered)
	:	m_min(glm::vec3(0.0f)),
		m_max(glm::vec3(0.0f)),
		m_position(position),
		m_dimension(dimension),
		m_scale(scale),
		m_halfDimension(glm::vec3(0.0f)),
		m_isCentered(isCentered)
{
	GenerateBounds();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
AABounds3D::~AABounds3D()
{

}


/*******************************************************************************************************************
	A function that disables the bounding box
*******************************************************************************************************************/
void AABounds3D::Disable()
{
	Update(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
}


/*******************************************************************************************************************
	A function that updates the bounding box providing any changes have happened
*******************************************************************************************************************/
void AABounds3D::Update(const glm::vec3& position, const glm::vec3& dimension, const glm::vec3& scale)
{
	//--- Check if a change has happened first before updating data
	if (m_position != position || m_dimension != dimension || m_scale != scale) {
		
		m_position	= position;
		m_dimension = dimension;
		m_scale		= scale;

		GenerateBounds();
	}
}


/*******************************************************************************************************************
	A function that generates the bounding box min and max extents
*******************************************************************************************************************/
void AABounds3D::GenerateBounds()
{
	if (m_isCentered) {

		//--- Position the bound around the model's origin (0.0, 0.5, 0.0) (center + 0.5 on Y axis)
		glm::vec3 halvedAndScaledDimension	= m_dimension * m_scale / 2.0f;
		float scaledHeight					= m_dimension.y * m_scale.y;

		m_min.x = m_position.x - halvedAndScaledDimension.x;
		m_min.y = m_position.y;
		m_min.z = m_position.z - halvedAndScaledDimension.z;

		m_max.x = m_position.x + halvedAndScaledDimension.x;
		m_max.y = m_position.y + scaledHeight;
		m_max.z = m_position.z + halvedAndScaledDimension.z;

		m_halfDimension = glm::vec3(halvedAndScaledDimension.x, scaledHeight, halvedAndScaledDimension.z);
	}
	else {
		//--- Position the bound around the model's origin (0.5, 0.5, -0.5) (bottom left corner)
		m_min.x = m_position.x;
		m_min.y = m_position.y;
		m_min.z = m_position.z - m_dimension.z * m_scale.z;

		m_max.x = m_position.x + m_dimension.x * m_scale.x;
		m_max.y = m_position.y + m_dimension.y * m_scale.y;
		m_max.z = m_position.z;
	}
}


/*******************************************************************************************************************
	A function that checks for collision between this bounding box and another
*******************************************************************************************************************/
bool AABounds3D::IsColliding(const AABounds3D& other) const
{	
	return ((m_min.x <= other.m_max.x && other.m_min.x <= m_max.x) &&
			(m_min.y <= other.m_max.y && other.m_min.y <= m_max.y) &&
			(m_min.z <= other.m_max.z && other.m_min.z <= m_max.z));
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec3& AABounds3D::GetDimension() const		{ return m_dimension; }
const glm::vec3& AABounds3D::GetPosition() const		{ return m_position; }
const glm::vec3& AABounds3D::GetMin() const				{ return m_min; }
const glm::vec3& AABounds3D::GetMax() const				{ return m_max; }
const glm::vec3& AABounds3D::GetHalfDimension() const	{ return m_halfDimension; }