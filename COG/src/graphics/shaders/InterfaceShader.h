#pragma once

/*******************************************************************************************************************
	InterfaceShader.h, InterfaceShader.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A derived class of base class Shader. The main shader used for all the 2D interface objects, e.g. buttons.

	[Features]
	Shader data only get's updated when changes have happened.

	[Upcoming]
	Motion/gaussian blur.
	Post processing effects - fade outs/screen tints, etc.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "graphics/shaders/UniformBlocks.h"

class InterfaceShader : public Shader {

public:
	InterfaceShader(const std::string& vertex, const std::string& fragment);
	virtual ~InterfaceShader();

public:
	void SetInstanceData(Transform* transform, Texture* texture);

private:
	virtual void GetAllUniforms()			override;
	virtual void SetPermanentAttributes()	override;

private:
	bool SetMatrixData(Transform* transform);
	bool SetTextureData(Texture* texture);
};