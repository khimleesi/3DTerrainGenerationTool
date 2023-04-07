#pragma once
/*******************************************************************************************************************
	RenderTarget.h, RenderTarget.cpp
	Created by Kim Kane
	Last updated: 12/04/2018

	Encapsulates all necessary procedures needed to generate a FBO, allowing us to render the scene to a texture
	or render buffer object.

	[Features]
	Supports depth and color attachment's.
	Supports render buffer attachment's.
	All FBO's, RBO's and textures generated get stored in memory for re-use.
	All necessary attributes already setup - just call BeginScene and EndScene to render to texture/RBO.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <string>
#include "graphics/Texture.h"

class RenderTarget {

public:
	RenderTarget(const std::string& tag, const glm::vec2& dimensions, bool color, bool depth, bool depthBuffer);
	~RenderTarget();

public:
	void BeginScene(bool perspective);
	void EndScene();

public:
	Texture* GetColorTexture();
	Texture* GetDepthTexture();

private:
	void	CreateScene(bool color, bool depth, bool depthBuffer);
	Texture CreateTextureAttachment(const std::string& tag, int attachment);

private:
	std::string		m_tag;
	int				m_width, m_height;
	Texture			m_colorTexture;
	Texture			m_depthTexture;
};