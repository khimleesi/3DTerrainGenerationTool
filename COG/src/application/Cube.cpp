#include "Cube.h"
#include "utilities/Log.h"
#include "managers/ResourceManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Cube::Cube(const std::string& tag)
	:	Primitive(tag + ".cube")

{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Cube::~Cube()
{

}


/*******************************************************************************************************************
	Function that creates a cube primitive of any size (desired size passed to function)
*******************************************************************************************************************/
bool Cube::Create(float size)
{
	std::vector<GLfloat> vertices = {
		
		//--- Front
		-size, -size,  size,
		 size, -size,  size,
		 size,  size,  size,
		-size,  size,  size,

		//--- Back
		-size, -size, -size,
		 size, -size, -size,
		 size,  size, -size,
		-size,  size, -size,
	};

	std::vector<GLuint> indices = {
		
		//--- Front
		0, 1, 2,
		2, 3, 0,
		
		//--- Right
		1, 5, 6,
		6, 2, 1,
		
		//--- Back
		7, 6, 5,
		5, 4, 7,
		
		//--- Left
		4, 0, 3,
		3, 7, 4,
		
		//--- Bottom
		4, 5, 1,
		1, 0, 4,
	
		//--- Top
		3, 2, 6,
		6, 7, 3,
	};

	if (!Resource::Instance()->AddBuffers(m_tag, true)) { return false; }

	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Push(vertices, VertexBuffer::LAYOUT_POSITION, false);
		Resource::Instance()->GetEBO(m_tag)->Push(indices);
	Resource::Instance()->GetVAO(m_tag)->Unbind();

	return true;
}


/*******************************************************************************************************************
	Function that renders the cube primitive to the screen
*******************************************************************************************************************/
void Cube::Render()
{
	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetEBO(m_tag)->Render();
}