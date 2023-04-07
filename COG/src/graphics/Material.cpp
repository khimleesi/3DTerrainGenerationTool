#include "Material.h"
#include "utilities/Log.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Material::Material(const std::string& diffuse, const std::string& normal, const std::string& specular, const std::string& emissive)
	:	m_isReflective(false), m_isGlowing(false), m_isNormalMapped(false), m_shininess(s_defaultShininess)
{
	Load(diffuse, normal, specular, emissive);
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Material::~Material()
{

}


/*******************************************************************************************************************
	Function that loads all startup procedures
*******************************************************************************************************************/
void Material::Load(const std::string& diffuse, const std::string& normal, const std::string& specular, const std::string& emissive)
{
	//--- Add the diffuse texture to our texture map, providing the string is not empty
	AddTexture(Shader::TEXTURE_DIFFUSE, diffuse);

	//--- Add the additional textures to our texture map and switch them on in the shader, providing the strings' aren't empty
	if (AddTexture(Shader::TEXTURE_NORMAL, normal))		{ m_isNormalMapped	= true; }
	if (AddTexture(Shader::TEXTURE_SPECULAR, specular))	{ m_isReflective	= true; }
	if (AddTexture(Shader::TEXTURE_EMISSIVE, emissive)) { m_isGlowing		= true; }
}


/*******************************************************************************************************************
	Function that checks if a texture string is empty and adds it to the m_textures map if not
*******************************************************************************************************************/
bool Material::AddTexture(Shader::TextureUnit unit, const std::string& texture)
{
	if (texture.empty())	{ return false; }
	else					{ m_textures.try_emplace(unit, texture, Shader::GetTextureUnit(unit)); }

	return true;
}


/*******************************************************************************************************************
	Function that binds all the textures within the m_textures map
*******************************************************************************************************************/
void Material::Bind()
{
	for (auto& texture : m_textures) { GetValue(texture).Bind(); }
}


/*******************************************************************************************************************
	Function that unbinds all the textures within the m_textures map
*******************************************************************************************************************/
void Material::Unbind()
{
	for (auto& texture : m_textures) { GetValue(texture).Unbind(); }
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
float Material::GetShininess() const	{ return m_shininess; }

Texture* Material::GetDiffuse()		{ return &m_textures[Shader::TEXTURE_DIFFUSE]; }
Texture* Material::GetEmissive()	{ return &m_textures[Shader::TEXTURE_EMISSIVE]; }
Texture* Material::GetSpecular()	{ return &m_textures[Shader::TEXTURE_SPECULAR]; }
Texture* Material::GetNormalMap()	{ return &m_textures[Shader::TEXTURE_NORMAL]; }

bool Material::IsReflective() const		{ return m_isReflective; }
bool Material::IsGlowing() const		{ return m_isGlowing; }
bool Material::IsNormalMapped() const	{ return m_isNormalMapped; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Material::SetShininess(float shininess) { m_shininess = shininess; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
float Material::s_defaultShininess = 64.0f;