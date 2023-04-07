#include "VertexArray.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
VertexArray::VertexArray()
	:	m_arrayObject(0)
{
	GenerateArrayObject();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
VertexArray::~VertexArray()
{
	COG_GLCALL(glDeleteVertexArrays(1, &m_arrayObject));

	COG_LOG("[VERTEX ARRAY] Vertex array object destroyed: ", m_arrayObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Binds the vertex array object ID & makes it the active array object
*******************************************************************************************************************/
void VertexArray::Bind() const
{
	COG_GLCALL(glBindVertexArray(m_arrayObject));
}


/*******************************************************************************************************************
	Unbinds the vertex array object ID & makes it disactive
*******************************************************************************************************************/
void VertexArray::Unbind() const
{
	COG_GLCALL(glBindVertexArray(0));
}


/*******************************************************************************************************************
	Generate the vertex array objects ID
*******************************************************************************************************************/
void VertexArray::GenerateArrayObject()
{
	COG_GLCALL(glGenVertexArrays(1, &m_arrayObject));

	COG_LOG("[VERTEX ARRAY] Vertex array object created: ", m_arrayObject, LOG_MEMORY);
}