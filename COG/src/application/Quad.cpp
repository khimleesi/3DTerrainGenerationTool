#include "Quad.h"
#include "managers/ResourceManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Quad::Quad(const std::string& tag)
	:	Primitive(tag + ".quad")
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Quad::~Quad() 
{
	
}


/*******************************************************************************************************************
	Function that creates a quad primitive
*******************************************************************************************************************/
bool Quad::Create(const std::string& tag)
{
	m_tag = tag + ".quad";

	std::vector<GLuint> vertices = {

		//--- Bottom left
		0, 0, 0,

		//-- Top Left

		0, 1, 0,
		//--- Bottom right

		1, 0, 0,
		//--- Top right
		1, 1, 0,
	};
	
	std::vector<GLfloat> textureCoords = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	//--- Does this quad already have buffers for the objects tag? If so, re-use these instead
	if (!Resource::Instance()->AddBuffers(m_tag, false, true)) { return false; }

	//--- Otherwise generate new buffers for the quad and push the following data to the GPU:
	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Push(vertices, VertexBuffer::LAYOUT_POSITION, false, GL_UNSIGNED_INT);
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_UV)->Push(textureCoords, VertexBuffer::LAYOUT_UV, false);
	Resource::Instance()->GetVAO(m_tag)->Unbind();

	return true;
}


/*******************************************************************************************************************
	Function that renders the quad primitive to the screen
*******************************************************************************************************************/
void Quad::Render()
{
	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Render(GL_TRIANGLE_STRIP);
}