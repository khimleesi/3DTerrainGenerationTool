#include "Minimap.h"
#include "graphics/shaders/InterfaceShader.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Minimap::Minimap(const std::string& tag, const Transform& transform)
	:	Interface(tag, transform),
		m_quad(tag),
		m_renderTarget(tag, m_transform.GetDimensions(), true, false, false)
{
	Load();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Minimap::~Minimap()
{

}


/*******************************************************************************************************************
	A function that loads all necessary startup procedures for the minimap
*******************************************************************************************************************/
void Minimap::Load()
{
	m_quad.Create(m_tag);
}


/*******************************************************************************************************************
	A function that renders the minimap to the screen
*******************************************************************************************************************/
void Minimap::Render(Shader* shader)
{
	if (InterfaceShader* interfaceShader = Downcast<InterfaceShader>(shader)) {

		const auto texture = m_renderTarget.GetColorTexture();

		if (texture) {
			
			interfaceShader->SetInstanceData(&m_transform, texture);

			texture->Bind();
				m_quad.Render();
			texture->Unbind();
		}
	}
}


/*******************************************************************************************************************
	A function that updates the minimap
*******************************************************************************************************************/
void Minimap::Update()
{
	m_transform.Update();
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
RenderTarget* Minimap::GetRenderTarget() { return &m_renderTarget; }