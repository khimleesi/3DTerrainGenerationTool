#pragma once
/*******************************************************************************************************************
	RenderBuffer.h, RenderBuffer.cpp
	Created by Kim Kane
	Last updated: 12/04/2018
	Class finalized: 12/04/2018

	Generates an RBO to be used in conjunction with an FBO.

	[Features]
	Nothing at present.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	The RBO is made to be encapsulated into some render target, as you will need to setup
	an FBO and/or textures.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>

class RenderBuffer {

public:
	RenderBuffer();
	~RenderBuffer();

public:
	void Bind() const;
	void Unbind() const;

public:
	void CreateStorage(int width, int height);

private:
	RenderBuffer(RenderBuffer const&)	= delete;
	void operator=(RenderBuffer const&) = delete;

private:
	void GenerateBufferObject();

private:
	GLuint m_renderBufferObject;
};