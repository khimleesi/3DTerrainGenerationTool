#pragma once

/*******************************************************************************************************************
	VertexBuffer.h, VertexBuffer.cpp
	Created by Kim Kane
	Last updated: 10/03/2018
	Class finalized: 02/04/2018

	Generates a VBO to send vertex data to the GPU.

	[Features]
	Supports an std::vector container of T data to send to the GPU, where T is templated data.
	Also added support for common vertex data - See PackedVertex struct within this class.
	Ability to switch between render modes at run time and push dynamic/static data to the GPU.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	All OpenGL specific's in this program follow the same pattern for consistency.
	This is as follows:

	1) Bind the object
	2) Push data to GPU / or update the object in some way
	3) Render the object
	4) Unbind the object (only needed in some cases*)

	*Not all objects need to be unbound after use - primarily VAO's. VAO's should be bound/unbound upon
	creation or updating, but when rendering a VAO it is not essential that we unbind, and unneccessary unbinding
	has an adverse affect on performance.
	However, thing's like textures need to be bound and unbound every frame - this is because our shader will still
	have the previous object's texture bound (especially if you are using the one texture unit!).
	This obviously affects performance ever so slightly and so a better option would be
	to create a massive batch renderer, which is a long process and one I didn't have time for.
	See: https://www.youtube.com/watch?v=1x4hrdS97cI for more information on these.
	Another way would be to use OpenGL texture arrays - but I personally didn't want to use them, as when
	I was researching them I discovered they were not as great as I thought and the general consensus
	amongst experienced programmers is that they should be avoided.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <vector>
#include <map>

#include "utilities/Log.h"

class VertexBuffer {

public:
	enum LayoutType : unsigned int { LAYOUT_POSITION, LAYOUT_UV, LAYOUT_NORMAL, LAYOUT_TANGENT, LAYOUT_BITANGENT };

public:
	VertexBuffer();
	~VertexBuffer();

public:
	struct PackedVertex {
		glm::vec3 position;
		glm::vec2 textureCoord;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		bool operator<(const PackedVertex that) const {
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
		};
	};

public:
	void Bind() const;
	void Unbind() const;

public:
	void Render(GLenum mode = GL_TRIANGLES) const;

public:
	bool Push(const std::vector<PackedVertex>& data, bool dynamic);
	
public:
	template <typename T> bool Push(const std::vector<T>& data, LayoutType layoutType, bool dynamic, int dataType = GL_FLOAT);
	template <typename T> bool Update(const std::vector<T>& data);

private:
	VertexBuffer(VertexBuffer const&)	= delete;
	void operator=(VertexBuffer const&)	= delete;

private:
	void GenerateBufferObject();

private:
	void DefineAttributeData(LayoutType layoutType, unsigned int stride = 0, size_t offset = 0, int dataType = GL_FLOAT);

private:
	GLuint			m_vertexBufferObject;
	unsigned int	m_vertexCount;

private:
	static std::map<LayoutType, int> s_bufferElements;
};


/*******************************************************************************************************************
	A template function that pushes the user-defined passed in data to the GPU for rendering
*******************************************************************************************************************/
template <typename T> bool VertexBuffer::Push(const std::vector<T>& data, LayoutType layoutType, bool dynamic, int dataType)
{
	//--- Make sure we have data before doing anything
	if (data.empty()) {
		COG_LOG("[BUFFER] Model vertex data vector container is empty", COG_LOG_EMPTY, LOG_ERROR); return false;
	}

	//--- Bind the VBO
	Bind();

	//--- Get the number of vertices
	m_vertexCount = data.size() / s_bufferElements[layoutType];

	//--- Push the data to the GPU
	COG_GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data.front(), (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	//--- Set up and enable the attributes for this data
	DefineAttributeData(layoutType, 0, 0, dataType);

	return true;
}


/*******************************************************************************************************************
	A template function that updates already existing data stored within the GPU
*******************************************************************************************************************/
template <typename T> bool VertexBuffer::Update(const std::vector<T>& data)
{
	//--- Make sure we have data before doing anything
	if (data.empty()) {
		COG_LOG("[BUFFER] Model vertex data vector container is empty", COG_LOG_EMPTY, LOG_ERROR); return false;
	}

	//--- Bind VBO
	Bind();

	//--- Push this new data to GPU
	COG_GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), &data.front()));

	//--- Unbind the VBO
	Unbind();

	return true;
}