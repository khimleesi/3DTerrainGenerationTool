#include "UniformBuffer.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
UniformBuffer::UniformBuffer()
	:	m_uniformBufferObject(0)
{
	GenerateBufferObject();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
UniformBuffer::~UniformBuffer()
{
	COG_GLCALL(glDeleteBuffers(1, &m_uniformBufferObject));

	COG_LOG("[UNIFORM BUFFER] Uniform buffer object destroyed: ", m_uniformBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	A function that pushes all the uniform buffer data to the GPU
*******************************************************************************************************************/
void UniformBuffer::Push(GLsizeiptr byteSize, GLuint binding, bool dynamic)
{
	//--- Bind the UBO
	Bind();

	//--- Push the initial data to the GPU (in this case we are only interested in the size of the data)
	COG_GLCALL(glBufferData(GL_UNIFORM_BUFFER, byteSize, NULL, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	//--- Unbind the UBO
	Unbind();

	//--- Bind this UBO to the binding number we have selected
	COG_GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_uniformBufferObject));
}


/*******************************************************************************************************************
	Generate the buffer objects ID
*******************************************************************************************************************/
void UniformBuffer::GenerateBufferObject()
{
	COG_GLCALL(glGenBuffers(1, &m_uniformBufferObject));

	COG_LOG("[UNIFORM BUFFER] Uniform buffer object created: ", m_uniformBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Binds the uniform buffer object ID & makes it the active buffer
*******************************************************************************************************************/
void UniformBuffer::Bind() const
{
	COG_GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferObject));
}


/*******************************************************************************************************************
	Unbinds the uniform buffer object ID & makes it disactive
*******************************************************************************************************************/
void UniformBuffer::Unbind() const
{
	COG_GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}