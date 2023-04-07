#include "TextureCache.h"
#include "utilities/Log.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
TextureCache::TextureCache()
{

}


/*******************************************************************************************************************
	Default Destructor
*******************************************************************************************************************/
TextureCache::~TextureCache()
{

}


/*******************************************************************************************************************
	A function that destroy's all OpenGL texture ID's from memory and removes them from the texture cache
*******************************************************************************************************************/
void TextureCache::Unload() {

	static unsigned int textureCount = s_textures.size();

	COG_LOG("[RESOURCE] s_textures map size before deletion: ", textureCount, LOG_RESOURCE);

	for (auto& texture : s_textures)
	{
		COG_LOG("[RESOURCE] Deleting texture from s_textures map: "
			+ GetKey(texture) + ", OpenGL texture ID: ", GetValue(texture), LOG_MEMORY);

		COG_GLCALL(glDeleteTextures(1, &GetValue(texture)));

		textureCount--;

		if (textureCount == 0) { s_textures.clear(); break; }
	}

	COG_LOG("[RESOURCE] s_textures map size after deletion: ", textureCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that adds a texture ID to the texture cache
*******************************************************************************************************************/
void TextureCache::AddTexture(const std::string& tag, GLuint id)
{
	s_textures.try_emplace(tag, id);

	COG_LOG("[RESOURCE] Texture added to s_textures map: ", tag.c_str(), LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that checks if a texture ID already exists in the texture cache, returns true if so
*******************************************************************************************************************/
bool TextureCache::FindTexture(const std::string& tag)
{
	auto texture = s_textures.find(tag);

	return texture != s_textures.end();
}


/*******************************************************************************************************************
	A function that gets a texture ID from the texture cache
*******************************************************************************************************************/
const GLuint& TextureCache::GetTexture(const std::string& tag)
{	
	return s_textures.at(tag);
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
TextureCache::Cache TextureCache::s_textures;