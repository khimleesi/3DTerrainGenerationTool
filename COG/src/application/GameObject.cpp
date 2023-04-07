#include "GameObject.h"
#include "application/GameComponent.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
GameObject::GameObject(const std::string& tag, const Transform& transform)
	:	m_tag(tag),
		m_transform(transform),
		m_isAlive(true),
		m_isActive(true)
{

}

GameObject::GameObject()
	:	m_tag("null"),
		m_transform(),
		m_isAlive(true),
		m_isActive(true)
{

}

/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
GameObject::~GameObject()
{

}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Transform* GameObject::GetTransform()			{ return &m_transform; }
const std::string& GameObject::GetTag() const	{ return m_tag; }
bool GameObject::IsActive() const				{ return m_isActive; }
bool GameObject::IsAlive() const				{ return m_isAlive; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void GameObject::SetActive(bool isActive)	{ m_isActive = isActive; }
void GameObject::SetAlive(bool isAlive)		{ m_isAlive = isAlive; }