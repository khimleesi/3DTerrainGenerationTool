#pragma once
/*******************************************************************************************************************
	Minimap.h, Minimap.cpp
	Created by Kim Kane
	Last updated: 28/04/2018

	A derived class of base class Interface. A minimap contains a render target (FBO) allowing you to render
	the terrain to a texture and then project it back on the screen.
	
	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing.

*******************************************************************************************************************/
#include "Interface.h"
#include "graphics/buffers/RenderTarget.h"
#include "application/Quad.h"

class Minimap : public Interface {

public:
	Minimap(const std::string& tag, const Transform& transform);
	virtual ~Minimap();

public:
	virtual void Render(Shader* shader) override;
	virtual void Update()				override;

public:
	RenderTarget* GetRenderTarget();

private:
	void Load();

private:
	Quad			m_quad;
	RenderTarget	m_renderTarget;
};