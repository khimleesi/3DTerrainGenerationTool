#include "AABounds2D.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
AABounds2D::AABounds2D(const glm::vec2& position, const glm::vec2& dimension)
	:	m_position(position),
		m_dimension(dimension)
{
	GenerateBounds();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
AABounds2D::~AABounds2D()
{

}


/*******************************************************************************************************************
	A function that disables the bounding box
*******************************************************************************************************************/
void AABounds2D::Disable()
{
	Update(glm::vec2(0.0f), glm::vec2(0.0f));
}


/*******************************************************************************************************************
	A function that updates the bounding box providing any changes have happened
*******************************************************************************************************************/
void AABounds2D::Update(const glm::vec2& position, const glm::vec2& dimension)
{
	//--- Check if any changes have happened first before updating data
	if (m_position != position || m_dimension != dimension) {

			m_position = position;
			m_dimension = dimension;

			GenerateBounds();
	}
}


/*******************************************************************************************************************
	A function that generates the bounding box's min and max extents
*******************************************************************************************************************/
void AABounds2D::GenerateBounds()
{
	m_min.x = m_position.x;
	m_min.y = m_position.y;

	m_max.x = (m_position.x + m_dimension.x);
	m_max.y = (m_position.y + m_dimension.y);
}


/*******************************************************************************************************************
	A function that checks for collision between this bounding box and another
*******************************************************************************************************************/
bool AABounds2D::IsColliding(const AABounds2D& other) const
{
	return ((m_min.x <= other.m_max.x && other.m_min.x <= m_max.x) &&
			(m_min.y <= other.m_max.y && other.m_min.y <= m_max.y));
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec2& AABounds2D::GetDimension() const	{ return m_dimension; }
const glm::vec2& AABounds2D::GetPosition() const	{ return m_position; }