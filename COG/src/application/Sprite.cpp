#include "Sprite.h"
#include "graphics/shaders/Shader.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Sprite::Sprite(const std::string& tag, const std::string& texture, bool isAnimated, unsigned int rows)
	:	m_texture(texture, Shader::GetTextureUnit(Shader::TEXTURE_INTERFACE)),
		m_quad(tag),
		m_isAnimated(isAnimated)
{
	Create(tag,texture, isAnimated, rows);
}


Sprite::Sprite()
	: m_isAnimated(false)
{

}

bool Sprite::Create(const std::string & tag, const std::string & texture, bool isAnimated, unsigned int rows)
{
	m_quad.Create(tag);
	m_texture.LoadTexture(texture, Shader::GetTextureUnit(Shader::TEXTURE_INTERFACE));
	m_texture.SetMirrored(true);
	m_isAnimated = isAnimated;

	if (m_isAnimated) { m_texture.SetRows(rows); }

	return File::Instance()->Save("Assets\\GameObjects\\Sprites\\" + tag + ".sprite.json", m_quad, m_texture, m_isAnimated);
}

bool Sprite::Load(const std::string & tag)
{
	if (!File::Instance()->Load("Assets\\GameObjects\\Sprites\\" + tag + ".sprite.json", m_quad, m_texture, m_isAnimated)) { return false; }
	m_quad.Create(tag);
	m_texture.LoadTexture(m_texture.GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_INTERFACE));
	m_texture.SetMirrored(true);

	return true;
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Sprite::~Sprite()
{

}



/*******************************************************************************************************************
	A function that renders a sprite to the screen
*******************************************************************************************************************/
void Sprite::Render()
{
	m_texture.Bind();
		m_quad.Render();
	m_texture.Unbind();
}


/*******************************************************************************************************************
	A function that updates a sprite
*******************************************************************************************************************/
void Sprite::Update(float frame)
{
	//--- Only update the texture if it is animated
	if (m_isAnimated) { SetAnimationFrame(frame); }
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Texture* Sprite::GetTexture()	{ return &m_texture; }
bool Sprite::IsAnimated() const { return m_isAnimated; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Sprite::SetAnimationFrame(float frame)						{ m_texture.SetIndex((int)frame); }
void Sprite::SetAnimated(bool isAnimated, unsigned int rows)	{ m_isAnimated = isAnimated; m_texture.SetRows(rows); }