#include "RenderTarget.h"
#include "managers/ResourceManager.h"
#include "utilities/Log.h"
#include "managers/ScreenManager.h"
#include "graphics/shaders/Shader.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
RenderTarget::RenderTarget(const std::string& tag, const glm::vec2& dimensions, bool color, bool depth, bool depthBuffer)
	:	m_tag(tag), m_width((int)dimensions.x), m_height((int)dimensions.y)
{
	CreateScene(color, depth, depthBuffer);
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
RenderTarget::~RenderTarget()
{

}


/*******************************************************************************************************************
	A function that generates necessary attributes for the scene we want to render to this render target
*******************************************************************************************************************/
void RenderTarget::CreateScene(bool color, bool depth, bool depthBuffer)
{
	//--- Add the FBO to our resource cache and set the necessary attachments
	//--- If the FBO, textures or RBO already exist, the resource manager will just use the ones already in memory
	Resource::Instance()->AddFBO(m_tag);
	Resource::Instance()->GetFBO(m_tag)->Bind();
	Resource::Instance()->GetFBO(m_tag)->SetAttachment(GL_COLOR_ATTACHMENT0);

	//--- Render to texture attachments - user passes in whatever ones they want to use
	if (color)	{ m_colorTexture = CreateTextureAttachment(".color", GL_COLOR_ATTACHMENT0); }
	if (depth)	{ m_depthTexture = CreateTextureAttachment(".depth", GL_DEPTH_ATTACHMENT); }
	
	if (depthBuffer) { 
		//--- Only create the depth buffer for this render target if it doesn't exist already
		if (Resource::Instance()->AddRBO(m_tag)) {
			Resource::Instance()->GetRBO(m_tag)->CreateStorage(m_width, m_height);
		}
	}

	//--- Once we have setup the FBO we unbind it like we do with everything else :)
	Resource::Instance()->GetFBO(m_tag)->Unbind();
}


/*******************************************************************************************************************
	A function that prepares our render target for drawing to
*******************************************************************************************************************/
void RenderTarget::BeginScene(bool perspective)
{
	//--- Bind the FBO and set the new viewport, don't forget to clear the screen
	
	Resource::Instance()->GetFBO(m_tag)->Bind();
	Screen::Instance()->SetCustomViewport(m_width, m_height);
	Screen::Instance()->BeginScene(0.0f, 0.0f, 0.0f);
	Screen::Instance()->PerspectiveView(true, true);
	
	//--- After calling this function, draw whatever you want and it will be stored in this render target
}


/*******************************************************************************************************************
	A function that completes the drawing to our render target and resets the scene back to its defaults
*******************************************************************************************************************/
void RenderTarget::EndScene()
{
	//--- Call this function once you have finished drawing the scene you want stored in this render target
	Resource::Instance()->GetFBO(m_tag)->Unbind();
	Screen::Instance()->PerspectiveView(true, false);
	Screen::Instance()->SetDefaultViewport();

	//--- Begin drawing normally again to the default frame buffer...
}


/*******************************************************************************************************************
	A helper function that returns a new FBO texture based on parameters passed to function
*******************************************************************************************************************/
Texture RenderTarget::CreateTextureAttachment(const std::string& tag, int attachment)
{	
	//--- Note I am always using the interface texture unit in this case, but this could be made generic later
	return Texture(attachment, m_width, m_height, Shader::GetTextureUnit(Shader::TEXTURE_INTERFACE), m_tag + tag);
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Texture* RenderTarget::GetColorTexture() { return &m_colorTexture; }
Texture* RenderTarget::GetDepthTexture() { return &m_depthTexture; }