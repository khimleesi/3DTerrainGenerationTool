#pragma once

/*******************************************************************************************************************
	BufferCache.h, BufferCache.cpp
	Created by Kim Kane
	Last updated: 06/04/2018

	Handles the correct memory storage for all our OpenGL buffers.

	[Features]
	Adds all VAO, VBO and EBO's to a cache upon creation, allowing re-use of ID's.
	Seperate cache for each buffer type - allowing generic use of buffers (vertex/index drawing).
	Multiple VBO support (for standard layout types - position, uv's, normals, etc.)

	[Upcoming]
	Better error checking features.
	Some code repetition - better handle on this, possibly templates.

	[Side Notes]
	All cache classes should be created within either a static class or a singleton.
	They should only be created within a permanent location already in memory, as their lifetime
	is meant to last throughout the duration of the program.
	If you want multiple instances - as you might want more than one cache for textures, etc. just remove
	the static keyword

*******************************************************************************************************************/
#include <map>
#include "graphics/buffers/VertexArray.h"
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/buffers/IndexBuffer.h"
#include "graphics/buffers/UniformBuffer.h"
#include "graphics/buffers/FrameBuffer.h"
#include "graphics/buffers/RenderBuffer.h"

class BufferCache {

public:
	BufferCache();

public:
	void Unload();
	~BufferCache();

public:
	bool AddBuffers(const std::string& tag, bool isIndexed,
					bool hasTextureCoords = false, bool hasNormals = false, bool hasTangentsAndBitangents = false);
	bool AddPackedBuffers(const std::string& tag, bool isIndexed);
	bool AddUBO(GLsizeiptr byteSize, GLuint binding, bool dynamic);
	bool AddFBO(const std::string& tag);
	bool AddRBO(const std::string& tag);

public:
	VertexArray*	GetVAO(const std::string& tag);
	VertexBuffer*	GetVBO(const std::string& tag, VertexBuffer::LayoutType layout);
	VertexBuffer*	GetPackedVBO(const std::string& tag);
	IndexBuffer*	GetEBO(const std::string& tag);
	UniformBuffer*	GetUBO(GLuint binding);
	FrameBuffer*	GetFBO(const std::string& tag);
	RenderBuffer*	GetRBO(const std::string& tag);

private:
	void UnloadVBOBuffers();
	void UnloadPackedVBOBuffers();
	void UnloadVAOBuffers();
	void UnloadEBOBuffers();
	void UnloadUBOBuffers();
	void UnloadFBOBuffers();
	void UnloadRBOBuffers();

private:
	bool AddVAO(const std::string& tag);
	void AddVBO(const std::string& tag, VertexBuffer::LayoutType layout);
	void AddPackedVBO(const std::string& tag);
	void AddEBO(const std::string& tag);

private:
	typedef std::map<std::string, VertexArray>										VAOCache;
	typedef std::map<std::string, std::map<VertexBuffer::LayoutType, VertexBuffer>>	VBOCache;
	typedef std::map<std::string, VertexBuffer>										PackedVBOCache;
	typedef std::map<std::string, IndexBuffer>										EBOCache;
	typedef std::map<GLuint, UniformBuffer>											UBOCache;
	typedef std::map<std::string, FrameBuffer>										FBOCache;
	typedef std::map<std::string, RenderBuffer>										RBOCache;

private:
	static VAOCache			s_vaoBuffers;
	static VBOCache			s_vboBuffers;
	static PackedVBOCache	s_vboPackedBuffers;
	static EBOCache			s_eboBuffers;
	static UBOCache			s_uboBuffers;
	static FBOCache			s_fboBuffers;
	static RBOCache			s_rboBuffers;
};