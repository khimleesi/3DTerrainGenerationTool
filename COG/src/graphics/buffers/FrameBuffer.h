#pragma once
/*******************************************************************************************************************
	FrameBuffer.h, FrameBuffer.cpp
	Created by Kim Kane
	Last updated: 12/04/2018
	Class finalized: 12/04/2018

	Generates an FBO to render to.

	[Features]
	Nothing at present.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	The FBO is made to be encapsulated into some render target, as you will need to setup
	textures and/or a render buffer also.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>

class FrameBuffer {

public:
	FrameBuffer();
	~FrameBuffer();

public:
	void SetAttachment(int attachment);

public:
	void Bind() const;
	void Unbind() const;

private:
	FrameBuffer(FrameBuffer const&)		= delete;
	void operator=(FrameBuffer const&)	= delete;

private:
	void GenerateBufferObject();

private:
	GLuint m_frameBufferObject;
};