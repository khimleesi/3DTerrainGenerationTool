#include "Interface.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Interface::Interface(const std::string& tag, const Transform& transform)
	:	m_tag(tag),
		m_transform(transform),
		m_isAlive(true),
		m_isActive(true)
{

}

Interface::Interface()
	:	m_tag("null"),
		m_transform(),
		m_isAlive(true),
		m_isActive(true)
{

}

/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Interface::~Interface()
{

}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Transform* Interface::GetTransform()			{ return &m_transform; }
const std::string& Interface::GetTag() const	{ return m_tag; }
bool Interface::IsActive() const				{ return m_isActive; }
bool Interface::IsAlive() const					{ return m_isAlive; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Interface::SetActive(bool isActive)	{ m_isActive = isActive; }
void Interface::SetAlive(bool isAlive)		{ m_isAlive = isAlive; }