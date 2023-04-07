#include "Skybox.h"
#include "managers/ScreenManager.h"
#include "graphics/shaders/SkyboxShader.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Skybox::Skybox(	const std::string& tag,
				const std::string& left,
				const std::string& right, 
				const std::string& top,
				const std::string& bottom, 
				const std::string& front,
				const std::string& back,
				float size)

	:	m_tag(tag + ".skybox"),
		m_size(size),
		m_texture{ m_tag, { left, right, top, bottom, front, back }, Shader::GetTextureUnit(Shader::TEXTURE_SKYBOX) },
		m_cube(m_tag)
{
	Load();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Skybox::~Skybox()
{

}


/*******************************************************************************************************************
	Function that loads the skybox
*******************************************************************************************************************/
void Skybox::Load()
{
	m_cube.Create(m_size);
}


/*******************************************************************************************************************
	Function that renders the skybox
*******************************************************************************************************************/
void Skybox::Render(Shader* shader)
{
	if (SkyboxShader* skyboxShader = Downcast<SkyboxShader>(shader)) {

		skyboxShader->SetInstanceData();

		//--- We want to render the skybox in perspective mode with depth enabled, but not write to the depth buffer
		Screen::Instance()->EnableDepthWriting(false);

		m_texture.Bind();
			m_cube.Render();
		m_texture.Unbind();

		//--- Enable writing to the depth buffer again once we have drawn the skybox
		Screen::Instance()->EnableDepthWriting(true);
	}
}