#pragma once

/*******************************************************************************************************************
	ResourceManager.h, ResourceManager.cpp
	Created by Kim Kane
	Last updated: 06/04/2018

	A singleton wrapper class that handles all memory management of our resources, as well as having ease of
	access wrapper methods to retrieve and alter data already stored in memory.

	[Features]
	Supports caching of fonts, textures and buffer objects to allow re-use of existing resources.
	Handles all memory de-allocation of resources, displaying messages in the debug window so we can see
	memory being allocated and de-allocated whilst debugging.
	Has various error checking features embedded into our cache classes (these aren't perfect, but will improve later).
	Resources are loaded when they are first called and don't get destroyed until the end of the game.

	[Upcoming]
	A more improved error checking system; exception handling, try/catch, throw. Popup dialog's when thing's go
	really wrong.

	[Side Notes]
	I built this class towards the end of making this program, as I had no idea how OpenGL worked or what
	kind of resources I would need/have. Therefore, there is definitely room for improvement and there
	are a bunch of thing's I'd like to change - but for now it's really simple and does the job.
	I am planning on re-building my engine over the summer and the resource management system will be the first
	thing I do - it's not easy doing it backwards :)

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <string>
#include "utilities/Singleton.h"
#include "cache/TextureCache.h"
#include "cache/FontCache.h"
#include "cache/BufferCache.h"

class ResourceManager {

public:
	friend class Singleton<ResourceManager>;

public:
	void Shutdown();

public:
	void AddFont(const std::string& tag, const std::map<GLchar, FontCache::Character>& glyphs);
	void AddTexture(const std::string& tag, GLuint id);

public:
	bool AddBuffers(const std::string& tag, bool isIndexed,
					bool hasTextureCoords = false, bool hasNormals = false, bool hasTangentsAndBitangents = false);
	bool AddPackedBuffers(const std::string& tag, bool isIndexed);

public:
	bool AddBinding(GLsizeiptr byteSize, GLuint binding, bool dynamic);
	bool AddFBO(const std::string& tag);
	bool AddRBO(const std::string& tag);

public:
	bool FindFont(const std::string& tag);
	bool FindTexture(const std::string& tag);

public:
	FontCache::Character*	GetFontCharacter(const std::string& tag, GLchar character);
	const GLuint&			GetTexture(const std::string& tag);

public:
	UniformBuffer*	GetBinding(GLuint binding);
	VertexBuffer*	GetVBO(const std::string& tag, VertexBuffer::LayoutType layout);
	VertexBuffer*	GetPackedVBO(const std::string& tag);
	IndexBuffer*	GetEBO(const std::string& tag);
	VertexArray*	GetVAO(const std::string& tag);
	FrameBuffer*	GetFBO(const std::string& tag);
	RenderBuffer*	GetRBO(const std::string& tag);

private:
	ResourceManager();
	ResourceManager(const ResourceManager&)				= delete;
	ResourceManager& operator=(const ResourceManager&)	= delete;

private:
	BufferCache		m_bufferCache;
	FontCache		m_fontCache;
	TextureCache	m_textureCache;
};

typedef Singleton<ResourceManager> Resource;