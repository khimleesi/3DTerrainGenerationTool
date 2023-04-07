#include "IndexBuffer.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
IndexBuffer::IndexBuffer()
	:	m_indexBufferObject(0),
		m_indexCount(0)
{
	GenerateBufferObject();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
IndexBuffer::~IndexBuffer()
{
	COG_GLCALL(glDeleteBuffers(1, &m_indexBufferObject));

	COG_LOG("[INDEX BUFFER] Index buffer object destroyed: ", m_indexBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Binds the index buffer object ID & makes it the active buffer
*******************************************************************************************************************/
void IndexBuffer::Bind() const
{
	COG_GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject));
}


/*******************************************************************************************************************
	Unbinds the index buffer object ID & makes it disactive
*******************************************************************************************************************/
void IndexBuffer::Unbind() const
{
	COG_GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


/*******************************************************************************************************************
	A function that renders the indexed buffer data to the screen
*******************************************************************************************************************/
void IndexBuffer::Render(GLenum mode) const
{
	COG_GLCALL(glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, nullptr));
}


/*******************************************************************************************************************
	A function that pushes all the passed in indexed data to the GPU for rendering
*******************************************************************************************************************/
bool IndexBuffer::Push(const std::vector<GLuint>& data, bool dynamic)
{
	//--- Make sure we have data before doing anything
	if (data.empty()) {
		COG_LOG("[INDEX BUFFER] Model index data vector container is empty", COG_LOG_EMPTY, LOG_ERROR); return false;
	}

	//--- Bind the index buffer object
	Bind();

	//--- Store the number of indices
	m_indexCount = data.size();
	
	//--- Push the data to the GPU
	COG_GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(GLuint), &data.front(), (dynamic)	? GL_DYNAMIC_DRAW
																											: GL_STATIC_DRAW));
	COG_LOG("[INDEX BUFFER] Pushed index data to the graphics card", COG_LOG_EMPTY, LOG_MESSAGE);

	return true;
}


/*******************************************************************************************************************
	Generate the buffer objects ID
*******************************************************************************************************************/
void IndexBuffer::GenerateBufferObject()
{
	COG_GLCALL(glGenBuffers(1, &m_indexBufferObject));

	COG_LOG("[INDEX BUFFER] Index buffer object created: ", m_indexBufferObject, LOG_MEMORY);
}