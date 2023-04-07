#include "ResourceManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
ResourceManager::ResourceManager()
{
	COG_LOG("[RESOURCE MANAGER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	A function that unloads all resources in memory and shuts down the resource manager
*******************************************************************************************************************/
void ResourceManager::Shutdown()
{
	m_bufferCache.Unload();
	m_fontCache.Unload();
	m_textureCache.Unload();

	COG_LOG("[RESOURCE MANAGER DESTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	A function that adds a new font to our font cache and relevant buffers needed to the buffer cache
*******************************************************************************************************************/
void ResourceManager::AddFont(const std::string& tag, const std::map<GLchar, FontCache::Character>& characters)
{
	m_fontCache.AddFont(tag, characters);

	//--- Fonts in this program will always have just 2 buffers - vertices and UV's - and won't be drawn indexed
	m_bufferCache.AddBuffers(tag, false, true);
}


/*******************************************************************************************************************
	A function that adds a new texture to the texture cache
*******************************************************************************************************************/
void ResourceManager::AddTexture(const std::string& tag, GLuint id)
{
	m_textureCache.AddTexture(tag, id);
}


/*******************************************************************************************************************
	A function that checks to see if a font already exists in our font cache, returns true if so
*******************************************************************************************************************/
bool ResourceManager::FindFont(const std::string& tag)
{
	return m_fontCache.FindFont(tag);
}


/*******************************************************************************************************************
	A function that checks to see if texture already exists in our texture cache, returns true if so
*******************************************************************************************************************/
bool ResourceManager::FindTexture(const std::string& tag)
{
	return m_textureCache.FindTexture(tag);
}


/*******************************************************************************************************************
	A function that returns a character bound to a font already in our font cache
*******************************************************************************************************************/
FontCache::Character* ResourceManager::GetFontCharacter(const std::string& tag, GLchar character)
{
	return m_fontCache.GetCharacter(tag, character);
}


/*******************************************************************************************************************
	A function that returns an OpenGL texture ID already in memory
*******************************************************************************************************************/
const GLuint& ResourceManager::GetTexture(const std::string& tag)
{
	return m_textureCache.GetTexture(tag);
}


/*******************************************************************************************************************
	A function that adds a VAO, single/multi VBO and EBO to our buffer cache
*******************************************************************************************************************/
bool ResourceManager::AddBuffers(const std::string& tag, bool isIndexed,
								 bool hasTextureCoords, bool hasNormals, bool hasTangentsAndBitangents)
{
	return m_bufferCache.AddBuffers(tag, isIndexed, hasTextureCoords, hasNormals, hasTangentsAndBitangents);
}


/*******************************************************************************************************************
	A function that adds a single VAO, packed VBO and EBO to our buffer cache
*******************************************************************************************************************/
bool ResourceManager::AddPackedBuffers(const std::string& tag, bool isIndexed)
{
	return m_bufferCache.AddPackedBuffers(tag, isIndexed);
}


/*******************************************************************************************************************
	A function that adds UBO to our buffer cache and binds it to the data passed in
*******************************************************************************************************************/
bool ResourceManager::AddBinding(GLsizeiptr byteSize, GLuint binding, bool dynamic)
{
	return m_bufferCache.AddUBO(byteSize, binding, dynamic);
}


/*******************************************************************************************************************
	A function that adds an FBO to our buffer cache
*******************************************************************************************************************/
bool ResourceManager::AddFBO(const std::string& tag)
{
	return m_bufferCache.AddFBO(tag);
}


/*******************************************************************************************************************
	A function that adds an RBO to our buffer cache
*******************************************************************************************************************/
bool ResourceManager::AddRBO(const std::string& tag)
{
	return m_bufferCache.AddRBO(tag);
}


/*******************************************************************************************************************
	A function that get's an RBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
RenderBuffer* ResourceManager::GetRBO(const std::string& tag)
{
	return m_bufferCache.GetRBO(tag);
}


/*******************************************************************************************************************
	A function that get's an FBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
FrameBuffer* ResourceManager::GetFBO(const std::string& tag)
{
	return m_bufferCache.GetFBO(tag);
}


/*******************************************************************************************************************
	A function that get's an already bound UBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
UniformBuffer* ResourceManager::GetBinding(GLuint binding)
{
	return m_bufferCache.GetUBO(binding);
}


/*******************************************************************************************************************
	A function that get's an already bound VBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
VertexBuffer* ResourceManager::GetVBO(const std::string& tag, VertexBuffer::LayoutType layout)
{
	return m_bufferCache.GetVBO(tag, layout);
}


/*******************************************************************************************************************
	A function that get's an already bound Packed VBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
VertexBuffer* ResourceManager::GetPackedVBO(const std::string& tag)
{
	return m_bufferCache.GetPackedVBO(tag);
}


/*******************************************************************************************************************
	A function that get's an already bound VAO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
VertexArray* ResourceManager::GetVAO(const std::string& tag)
{
	return m_bufferCache.GetVAO(tag);
}


/*******************************************************************************************************************
	A function that get's an already bound EBO in our buffer cache, returns nullptr if doesn't exist
*******************************************************************************************************************/
IndexBuffer* ResourceManager::GetEBO(const std::string& tag)
{
	return m_bufferCache.GetEBO(tag);
}