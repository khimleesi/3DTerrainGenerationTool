#include "VertexBuffer.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
VertexBuffer::VertexBuffer()
	:	m_vertexBufferObject(0),
		m_vertexCount(0)
{
	GenerateBufferObject();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
VertexBuffer::~VertexBuffer()
{
	COG_GLCALL(glDeleteBuffers(1, &m_vertexBufferObject));

	COG_LOG("[VERTEX BUFFER] Vertex buffer object destroyed: ", m_vertexBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Binds the vertex buffer object ID & makes it the active buffer
*******************************************************************************************************************/
void VertexBuffer::Bind() const
{
	COG_GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject));
}


/*******************************************************************************************************************
	Unbinds the vertex buffer object ID & makes it disactive
*******************************************************************************************************************/
void VertexBuffer::Unbind() const
{
	COG_GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


/*******************************************************************************************************************
	A function that renders the vertex buffer data to the screen
*******************************************************************************************************************/
void VertexBuffer::Render(GLenum mode) const
{
	COG_GLCALL(glDrawArrays(mode, 0, m_vertexCount));
}


/*******************************************************************************************************************
	A function that pushes interleaved vertex data to the GPU (there is also a template rendition of this function)
*******************************************************************************************************************/
bool VertexBuffer::Push(const std::vector<PackedVertex>& data, bool dynamic)
{
	//--- Make sure we have data before doing anything
	if (data.empty()) {
		COG_LOG("[BUFFER] Model vertex data vector container is empty", COG_LOG_EMPTY, LOG_ERROR); return false;
	}

	//--- Bind the VBO
	Bind();

	//--- Get the vertex count
	m_vertexCount = data.size();

	//--- Create the buffer store, passing in the total byte size of the data,
	//--- the data itself and whether to draw in static/dynamic mode
	COG_GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(PackedVertex), &data.front(), (dynamic)	? GL_DYNAMIC_DRAW
																											: GL_STATIC_DRAW));

	//--- Enable the vertex attribute pointers for the data, passing in the stride and the offset of each
	DefineAttributeData(LAYOUT_POSITION, sizeof(PackedVertex), offsetof(PackedVertex, position));
	DefineAttributeData(LAYOUT_UV, sizeof(PackedVertex), offsetof(PackedVertex, textureCoord));
	DefineAttributeData(LAYOUT_NORMAL, sizeof(PackedVertex), offsetof(PackedVertex, normal));
	DefineAttributeData(LAYOUT_TANGENT, sizeof(PackedVertex), offsetof(PackedVertex, tangent));
	DefineAttributeData(LAYOUT_BITANGENT, sizeof(PackedVertex), offsetof(PackedVertex, bitangent));

	//--- NOTE
	// We don't have to unbind the VBO after creating the buffer - as all VBO's are encased within a VAO in this program.
	// If for whatever reason we didn't want to use VAO's, we would bind/unbind the VBO's and EBO's outwith this function instead.
	// We want to keep unneccessary binding and unbinding minimal, as too much of it hinders performance.
	//---

	return true;
}


/*******************************************************************************************************************
	Function that defines an array of generic vertex attribute data & enables the layout location of the data
*******************************************************************************************************************/
void VertexBuffer::DefineAttributeData(LayoutType layoutType, unsigned int stride, size_t offset, int dataType)
{
	COG_GLCALL(glVertexAttribPointer(layoutType, s_bufferElements[layoutType], dataType, GL_FALSE, stride, (size_t*)offset));
	COG_GLCALL(glEnableVertexAttribArray(layoutType));
}


/*******************************************************************************************************************
	Generate the buffer objects ID
*******************************************************************************************************************/
void VertexBuffer::GenerateBufferObject()
{
	COG_GLCALL(glGenBuffers(1, &m_vertexBufferObject));

	COG_LOG("[VERTEX BUFFER] Vertex buffer object created: ", m_vertexBufferObject, LOG_MEMORY);
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
std::map<VertexBuffer::LayoutType, int> VertexBuffer::s_bufferElements =	{	{ LAYOUT_POSITION, 3 },
																				{ LAYOUT_UV, 2 },
																				{ LAYOUT_NORMAL, 3 },
																				{ LAYOUT_TANGENT, 3 },
																				{ LAYOUT_BITANGENT, 3 }
																			};