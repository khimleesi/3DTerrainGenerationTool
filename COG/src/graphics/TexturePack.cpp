#include "TexturePack.h"
#include "utilities/Log.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	[Textures] Constructor called when object is created
*******************************************************************************************************************/
TexturePack::TexturePack(const std::string& baseTexture,
						 const std::string& rTexture,
						 const std::string& gTexture,
						 const std::string& bTexture,
						 const std::string& blendMap)
{
	Load(baseTexture, rTexture, gTexture, bTexture, blendMap);
}


/*******************************************************************************************************************
	[Normal Maps] Constructor called when object is created
*******************************************************************************************************************/
TexturePack::TexturePack(const std::string& baseTextureNormal,
						 const std::string& rTextureNormal,
						 const std::string& gTextureNormal,
						 const std::string& bTextureNormal)
{
	Load(baseTextureNormal, rTextureNormal, gTextureNormal, bTextureNormal);
}


/*******************************************************************************************************************
	Function that loads all startup procedures
*******************************************************************************************************************/
void TexturePack::Load(	const std::string& baseTexture,
						const std::string& rTexture,
						const std::string& gTexture,
						const std::string& bTexture,
						const std::string& blendMap)
{
	//--- Create the textures and store them into our m_textures map so we can bind them later
	AddTexture(Shader::TEXTURE_BASE, baseTexture);
	AddTexture(Shader::TEXTURE_RED, rTexture);
	AddTexture(Shader::TEXTURE_GREEN, gTexture);
	AddTexture(Shader::TEXTURE_BLUE, bTexture);
	AddTexture(Shader::TEXTURE_BLENDMAP, blendMap);
}


/*******************************************************************************************************************
	Function that loads all startup procedures
*******************************************************************************************************************/
void TexturePack::Load( const std::string& baseTextureNormal,
						const std::string& rTextureNormal,
						const std::string& gTextureNormal,
						const std::string& bTextureNormal)
{
	//--- Create the textures and store them into our m_textures map so we can bind them later
	AddTexture(Shader::TEXTURE_BASE_NORMAL, baseTextureNormal + ".normal");
	AddTexture(Shader::TEXTURE_RED_NORMAL, rTextureNormal + ".normal");
	AddTexture(Shader::TEXTURE_GREEN_NORMAL, gTextureNormal + ".normal");
	AddTexture(Shader::TEXTURE_BLUE_NORMAL, bTextureNormal + ".normal");
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
TexturePack::~TexturePack()
{

}

/*******************************************************************************************************************
	Load diffuse textures for the first time
*******************************************************************************************************************/
void TexturePack::LoadDiffuse(const std::string & baseTexture, const std::string & rTexture, const std::string & gTexture, const std::string & bTexture, const std::string & blendMap)
{
	Load(baseTexture, rTexture, gTexture, bTexture, blendMap);
}


/*******************************************************************************************************************
	Load normal textures already saved previously
*******************************************************************************************************************/
void TexturePack::LoadNormalFromMap()
{
	if (!m_textures.empty()) {
		m_textures.at(Shader::TEXTURE_BASE_NORMAL).LoadTexture(m_textures.at(Shader::TEXTURE_BASE_NORMAL).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_BASE_NORMAL));
		m_textures.at(Shader::TEXTURE_RED_NORMAL).LoadTexture(m_textures.at(Shader::TEXTURE_RED_NORMAL).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_RED_NORMAL));
		m_textures.at(Shader::TEXTURE_GREEN_NORMAL).LoadTexture(m_textures.at(Shader::TEXTURE_GREEN_NORMAL).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_GREEN_NORMAL));
		m_textures.at(Shader::TEXTURE_BLUE_NORMAL).LoadTexture(m_textures.at(Shader::TEXTURE_BLUE_NORMAL).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_BLUE_NORMAL));
	}
}

/*******************************************************************************************************************
	Load diffuse textures already saved previously
*******************************************************************************************************************/
void TexturePack::LoadDiffuseFromMap()
{
	if (!m_textures.empty()) {
		m_textures.at(Shader::TEXTURE_BASE).LoadTexture(m_textures.at(Shader::TEXTURE_BASE).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_BASE));
		m_textures.at(Shader::TEXTURE_RED).LoadTexture(m_textures.at(Shader::TEXTURE_RED).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_RED));
		m_textures.at(Shader::TEXTURE_GREEN).LoadTexture(m_textures.at(Shader::TEXTURE_GREEN).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_GREEN));
		m_textures.at(Shader::TEXTURE_BLUE).LoadTexture(m_textures.at(Shader::TEXTURE_BLUE).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_BLUE));
		m_textures.at(Shader::TEXTURE_BLENDMAP).LoadTexture(m_textures.at(Shader::TEXTURE_BLENDMAP).GetTag(), Shader::GetTextureUnit(Shader::TEXTURE_BLENDMAP));
	}
}

/*******************************************************************************************************************
	Load normal textures for the first time
*******************************************************************************************************************/
void TexturePack::LoadNormal(const std::string & baseTextureNormal, const std::string & rTextureNormal, const std::string & gTextureNormal, const std::string & bTextureNormal)
{
	Load(baseTextureNormal, rTextureNormal, gTextureNormal, bTextureNormal);
}


/*******************************************************************************************************************
	Function that checks if a texture string is empty and adds it to the m_textures map if not
*******************************************************************************************************************/
bool TexturePack::AddTexture(Shader::TextureUnit unit, const std::string& texture)
{
	if (texture.empty())	{ return false; }
	else					{	std::string src = "Terrain\\" + texture + ".png";
								m_textures.try_emplace(unit, src, Shader::GetTextureUnit(unit));
								m_textures.at(unit).LoadTexture(src, Shader::GetTextureUnit(unit)); }

	return true;
}


/*******************************************************************************************************************
	Function that binds all the textures within the m_textures array
*******************************************************************************************************************/
void TexturePack::Bind() const
{
	for (auto& texture : m_textures) { GetValue(texture).Bind(); }
}


/*******************************************************************************************************************
	Function that unbinds all the textures within the m_textures array
*******************************************************************************************************************/
void TexturePack::Unbind() const
{
	for (auto& texture : m_textures) { GetValue(texture).Unbind(); }
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Texture* TexturePack::GetBlendMap() { return &m_textures[Shader::TEXTURE_BLENDMAP]; }