#include "Background.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"
#include "managers/FileManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Background::Background()
{

}

/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Background::~Background()
{

}

bool Background::Create(const std::string& tag, const Transform& transform, const std::string& texture)
{
	m_tag = tag + ".background";
	m_sprite.Create(tag, texture);
	m_transform = transform;

	return File::Instance()->Save("Assets\\GameObjects\\Backgrounds\\" + m_tag + ".json", m_tag, m_transform, m_sprite);
}

bool Background::Load(const std::string& tag)
{
	if (!File::Instance()->Load("Assets\\GameObjects\\Backgrounds\\" + tag + ".background.json", m_tag, m_transform, m_sprite)) { return false; }
	m_sprite.Load(tag);
	m_transform.SetDirty(true);

	return true;
}


/*******************************************************************************************************************
	A function that renders the button to the screen
*******************************************************************************************************************/
void Background::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		interfaceShader->SetInstanceData(&m_transform, m_sprite.GetTexture());

		m_sprite.Render();
	}
}


/*******************************************************************************************************************
	A function that updates the background
*******************************************************************************************************************/
void Background::Update()
{
	m_transform.Update();
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Sprite* Background::GetSprite() { return &m_sprite; }