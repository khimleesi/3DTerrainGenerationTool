#pragma once

/*******************************************************************************************************************
	UniformBuffer.h, UniformBuffer.cpp
	Created by Kim Kane
	Last updated: 10/03/2018
	Class finalized: 02/04/2018

	Generates a UBO to encapsulate shader uniforms in to one binding slot.

	[Features]
	Nothing fancy.

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
#include "utilities/Log.h"

class UniformBuffer {

public:
	UniformBuffer();
	~UniformBuffer();

public:
	void Bind() const;
	void Unbind() const;

public:
	void Push(GLsizeiptr byteSize, GLuint binding, bool dynamic = false);
	
public:	
	template <typename T> void Update(const T* data);

private:
	UniformBuffer(UniformBuffer const&)		= delete;
	void operator=(UniformBuffer const&)	= delete;

private:
	void GenerateBufferObject();

private:
	GLuint m_uniformBufferObject;
};


/*******************************************************************************************************************
	A template function that updates already existing data stored within the GPU
*******************************************************************************************************************/
template <typename T> void UniformBuffer::Update(const T* data)
{
	//--- Bind the UBO
	Bind();

	//--- Update the data
	COG_GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), data));

	//--- Unbind the UBO
	Unbind();
}