#include "BufferCache.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
BufferCache::BufferCache()
{

}


/*******************************************************************************************************************
	Default Destructor
*******************************************************************************************************************/
BufferCache::~BufferCache()
{

}


/*******************************************************************************************************************
	A function that unloads all buffers from memory
*******************************************************************************************************************/
void BufferCache::Unload()
{
	UnloadEBOBuffers();
	UnloadVBOBuffers();
	UnloadPackedVBOBuffers();
	UnloadVAOBuffers();
	UnloadUBOBuffers();
	UnloadFBOBuffers();
	UnloadRBOBuffers();
}


/*******************************************************************************************************************
	A function that unloads all UBO buffers stored in the UBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadUBOBuffers()
{
	static unsigned int uboCount = s_uboBuffers.size();
	COG_LOG("[RESOURCE] s_uboBuffers map size before deletion: ", uboCount, LOG_RESOURCE);

	for (auto& ubo : s_uboBuffers) {
		COG_LOG("[RESOURCE] UBO destroyed, binding: ", GetKey(ubo), LOG_RESOURCE);
		uboCount--;
	}
	if (uboCount == 0) { s_uboBuffers.clear(); }
	COG_LOG("[RESOURCE] s_uboBuffers map size after deletion: ", uboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all EBO buffers stored in the EBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadEBOBuffers()
{
	static unsigned int eboCount = s_eboBuffers.size();
	COG_LOG("[RESOURCE] s_eboBuffers map size before deletion: ", eboCount, LOG_RESOURCE);

	for (auto& ebo : s_eboBuffers) {
		COG_LOG("[RESOURCE] EBO destroyed: ", GetKey(ebo).c_str(), LOG_RESOURCE);
		eboCount--;
	}
	if (eboCount == 0) { s_eboBuffers.clear(); }
	COG_LOG("[RESOURCE] s_eboBuffers map size after deletion: ", eboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all VAO buffers stored in the VAO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadVAOBuffers()
{
	static unsigned int vaoCount = s_vaoBuffers.size();
	COG_LOG("[RESOURCE] s_vaoBuffers map size before deletion: ", vaoCount, LOG_RESOURCE);

	for (auto& vao : s_vaoBuffers) {
		COG_LOG("[RESOURCE] VAO destroyed: ", GetKey(vao).c_str(), LOG_RESOURCE);
		vaoCount--;
		if (vaoCount == 0) { s_vaoBuffers.clear(); break; }
	}
	COG_LOG("[RESOURCE] s_vaoBuffers map size after deletion: ", vaoCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all FBO buffers stored in the FBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadFBOBuffers()
{
	static unsigned int fboCount = s_fboBuffers.size();
	COG_LOG("[RESOURCE] s_fboBuffers map size before deletion: ", fboCount, LOG_RESOURCE);

	for (auto& fbo : s_fboBuffers) {
		COG_LOG("[RESOURCE] FBO destroyed: ", GetKey(fbo).c_str(), LOG_RESOURCE);
		fboCount--;
		if (fboCount == 0) { s_fboBuffers.clear(); break; }
	}
	COG_LOG("[RESOURCE] s_fboBuffers map size after deletion: ", fboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all RBO buffers stored in the RBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadRBOBuffers()
{
	static unsigned int rboCount = s_rboBuffers.size();
	COG_LOG("[RESOURCE] s_rboBuffers map size before deletion: ", rboCount, LOG_RESOURCE);

	for (auto& rbo : s_rboBuffers) {
		COG_LOG("[RESOURCE] RBO destroyed: ", GetKey(rbo).c_str(), LOG_RESOURCE);
		rboCount--;
		if (rboCount == 0) { s_rboBuffers.clear(); break; }
	}
	COG_LOG("[RESOURCE] s_rboBuffers map size after deletion: ", rboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all VBO buffers stored in the VBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadVBOBuffers()
{
	static unsigned int vboCount = s_vboBuffers.size();
	COG_LOG("[RESOURCE] s_vboBuffers map size before deletion: ", vboCount, LOG_RESOURCE);

	for (auto& vbo : s_vboBuffers) {
		
		for (auto& layout : vbo.second) {
			COG_LOG("[RESOURCE] VBO(s) destroyed: " + GetKey(vbo) + ", layout type destroyed: ", GetKey(layout), LOG_RESOURCE);
		}

		vboCount--;
		if (vboCount == 0) { s_vboBuffers.clear(); break; }
	}
	COG_LOG("[RESOURCE] s_vboBuffers map size after deletion: ", vboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that unloads all Packed VBO buffers stored in the Packed VBO cache (purely for debugging purposes)
*******************************************************************************************************************/
void BufferCache::UnloadPackedVBOBuffers()
{
	static unsigned int vboCount = s_vboPackedBuffers.size();
	COG_LOG("[RESOURCE] s_vboPackedBuffers map size before deletion: ", vboCount, LOG_RESOURCE);

	for (auto& vbo : s_vboPackedBuffers) {
		COG_LOG("[RESOURCE] Packed VBO destroyed: ", GetKey(vbo).c_str(), LOG_RESOURCE);
		vboCount--;
		if (vboCount == 0) { s_vboPackedBuffers.clear(); break; }
	}
	COG_LOG("[RESOURCE] s_vboPackedBuffers map size after deletion: ", vboCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that adds a VAO, single/multi VBO and EBO to our buffer cache
*******************************************************************************************************************/
bool BufferCache::AddBuffers(const std::string& tag, bool isIndexed,
							 bool hasTextureCoords, bool hasNormals, bool hasTangentsAndBitangents)
{
	if (!AddVAO(tag))				{ return false; }

	AddVBO(tag, VertexBuffer::LAYOUT_POSITION);

	if (hasTextureCoords)			{ AddVBO(tag, VertexBuffer::LAYOUT_UV); }
	if (hasNormals)					{ AddVBO(tag, VertexBuffer::LAYOUT_NORMAL); }
	if (hasTangentsAndBitangents)	{ AddVBO(tag, VertexBuffer::LAYOUT_TANGENT);  AddVBO(tag, VertexBuffer::LAYOUT_BITANGENT); }

	if (isIndexed)					{ AddEBO(tag); }

	return true;
}


/*******************************************************************************************************************
	A function that adds a VAO, packed VBO and EBO to our buffer cache
*******************************************************************************************************************/
bool BufferCache::AddPackedBuffers(const std::string& tag, bool isIndexed)
{
	if (!AddVAO(tag))	{ return false; }

	AddPackedVBO(tag);

	if (isIndexed)		{ AddEBO(tag); }

	return true;
}


/*******************************************************************************************************************
	A function that returns a packed VBO already in memory or returns nullptr if not found
*******************************************************************************************************************/
VertexBuffer* BufferCache::GetPackedVBO(const std::string& tag)
{
	auto vbo = s_vboPackedBuffers.find(tag);

	if (vbo != s_vboPackedBuffers.end()) {

		return &GetValue<VertexBuffer>(vbo);
	}

	COG_LOG("[BUFFER CACHE] Packed VBO doesn't exist for tag: ", tag.c_str(), LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that returns a VAO already in memory or returns nullptr if not found
*******************************************************************************************************************/
VertexArray* BufferCache::GetVAO(const std::string& tag)
{
	auto vao = s_vaoBuffers.find(tag);

	if (vao != s_vaoBuffers.end()) {
		
		return &GetValue<VertexArray>(vao);
	}

	COG_LOG("[BUFFER CACHE] VAO doesn't exist for tag: ", tag.c_str(), LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that returns a EBO already in memory or returns nullptr if not found
*******************************************************************************************************************/
IndexBuffer* BufferCache::GetEBO(const std::string& tag)
{
	auto ebo = s_eboBuffers.find(tag);

	if (ebo != s_eboBuffers.end()) {

		return &GetValue<IndexBuffer>(ebo);
	}

	COG_LOG("[BUFFER CACHE] EBO doesn't exist for tag: ", tag.c_str(), LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that returns a VBO already in memory, or returns nullptr if not found
*******************************************************************************************************************/
VertexBuffer* BufferCache::GetVBO(const std::string& tag, VertexBuffer::LayoutType layout)
{
	auto vbo = s_vboBuffers.find(tag);

	if (vbo != s_vboBuffers.end()) {

		auto type = vbo->second.find(layout);

		if (type != vbo->second.end()) {
		
			return &GetValue<VertexBuffer>(type);
		}
		else { COG_LOG("[BUFFER CACHE] Layout type doesn't exist for tag: " + tag + ", layout type: ", layout, LOG_ERROR); }
	}
	else { COG_LOG("[BUFFER CACHE] VBO doesn't exist for tag: ", tag.c_str(), LOG_ERROR); }

	return nullptr;
}


/*******************************************************************************************************************
	A function that returns a UBO currently bound in memory, returns nullptr if not found
*******************************************************************************************************************/
UniformBuffer* BufferCache::GetUBO(GLuint binding)
{
	auto ubo = s_uboBuffers.find(binding);

	if (ubo != s_uboBuffers.end()) {

		return &GetValue<UniformBuffer>(ubo);
	}

	COG_LOG("[BUFFER CACHE] UBO doesn't exist for binding: ", binding, LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that adds a UBO to the UBO cache, and binds the data passed in to this UBO
*******************************************************************************************************************/
bool BufferCache::AddUBO(GLsizeiptr byteSize, GLuint binding, bool dynamic)
{
	//--- Make sure the data has a base alignment of 16
	if (byteSize % 16 != 0) {
		COG_LOG("[BUFFER CACHE] UBO buffer not 16 byte aligned, current bytes: ", byteSize, LOG_ERROR);
		return false;
	}

	bool added = s_uboBuffers.try_emplace(binding).second;

	if (!added) {

		COG_LOG("[BUFFER CACHE] UBO buffer already exists for this binding, re-using: ", binding, LOG_RESOURCE);
		return false;
	}

	COG_LOG("[BUFFER CACHE] UBO binding added to buffer cache: ", binding, LOG_RESOURCE);

	//--- Bind the data to the UBO we generated
	s_uboBuffers[binding].Push(byteSize, binding, dynamic);

	return true;
}


/*******************************************************************************************************************
	A function that adds a FBO to the FBO cache, if it doesn't exist already
*******************************************************************************************************************/
bool BufferCache::AddFBO(const std::string& tag)
{
	bool added = s_fboBuffers.try_emplace(tag).second;

	if (!added) {

		COG_LOG("[BUFFER CACHE] FBO buffer already exists for this tag, re-using: ", tag.c_str(), LOG_RESOURCE);
		return false;
	}

	COG_LOG("[BUFFER CACHE] FBO added to buffer cache: ", tag.c_str(), LOG_RESOURCE);

	return true;
}


/*******************************************************************************************************************
	A function that adds an RBO to the RBO cache, if it doesn't exist already
*******************************************************************************************************************/
bool BufferCache::AddRBO(const std::string& tag)
{
	bool added = s_rboBuffers.try_emplace(tag).second;

	if (!added) {

		COG_LOG("[BUFFER CACHE] RBO buffer already exists for this tag, re-using: ", tag.c_str(), LOG_RESOURCE);
		return false;
	}
	
	COG_LOG("[BUFFER CACHE] RBO added to buffer cache: ", tag.c_str(), LOG_RESOURCE);

	return true;
}


/*******************************************************************************************************************
	A function that returns a FBO already in memory, returns nullptr if not found
*******************************************************************************************************************/
FrameBuffer* BufferCache::GetFBO(const std::string& tag)
{
	auto fbo = s_fboBuffers.find(tag);

	if (fbo != s_fboBuffers.end()) {

		return &GetValue<FrameBuffer>(fbo);
	}

	COG_LOG("[BUFFER CACHE] FBO doesn't exist for tag: ", tag.c_str(), LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that returns a RBO already in memory, returns nullptr if not found
*******************************************************************************************************************/
RenderBuffer* BufferCache::GetRBO(const std::string& tag)
{
	auto rbo = s_rboBuffers.find(tag);

	if (rbo != s_rboBuffers.end()) {

		return &GetValue<RenderBuffer>(rbo);
	}

	COG_LOG("[BUFFER CACHE] RBO doesn't exist for tag: ", tag.c_str(), LOG_ERROR);

	return nullptr;
}


/*******************************************************************************************************************
	A function that adds a VAO to the VAO cache
*******************************************************************************************************************/
bool BufferCache::AddVAO(const std::string& tag)
{
	bool added = s_vaoBuffers.try_emplace(tag).second;

	if (!added) {

		COG_LOG("[BUFFER CACHE] Buffers already exist for this object, re-using: ", tag.c_str(), LOG_RESOURCE);
		return false;
	}
	
	COG_LOG("[BUFFER CACHE] VAO added to buffer cache: ", tag.c_str(), LOG_RESOURCE);
	
	return true;
}


/*******************************************************************************************************************
	A function that adds an EBO to the EBO cache
*******************************************************************************************************************/
void BufferCache::AddEBO(const std::string& tag)
{
	s_eboBuffers.try_emplace(tag);

	COG_LOG("[BUFFER CACHE] EBO added to buffer cache: ", tag.c_str(), LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that adds a single VBO to the VBO cache
*******************************************************************************************************************/
void BufferCache::AddVBO(const std::string& tag, VertexBuffer::LayoutType layout)
{
	s_vboBuffers[tag].try_emplace(layout);

	COG_LOG("[BUFFER CACHE] VBO added to buffer cache: " + tag + ", at layout: ", layout, LOG_RESOURCE);
}


/*******************************************************************************************************************
	A function that adds a packed VBO to the packed VBO cache
*******************************************************************************************************************/
void BufferCache::AddPackedVBO(const std::string& tag)
{
	s_vboPackedBuffers.try_emplace(tag);

	COG_LOG("[BUFFER CACHE] Packed VBO added to buffer cache: ", tag.c_str(), LOG_RESOURCE);
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
BufferCache::VAOCache BufferCache::s_vaoBuffers;
BufferCache::VBOCache BufferCache::s_vboBuffers;
BufferCache::PackedVBOCache BufferCache::s_vboPackedBuffers;
BufferCache::EBOCache BufferCache::s_eboBuffers;
BufferCache::UBOCache BufferCache::s_uboBuffers;
BufferCache::FBOCache BufferCache::s_fboBuffers;
BufferCache::RBOCache BufferCache::s_rboBuffers;