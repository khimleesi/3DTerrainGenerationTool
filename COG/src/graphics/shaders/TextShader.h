#pragma once
/*******************************************************************************************************************
	TextShader.h, TextShader.cpp
	Created by Kim Kane
	Last updated: 11/04/2018

	A derived class of base class Shader. The main shader used for the in-game text.

	[Features]
	Supports FreeType text.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "Shader.h"

class TextShader : public Shader {

public:
	TextShader(const std::string& vertex, const std::string& fragment);
	virtual ~TextShader();

public:
	void SetInstanceData(Transform* transform, const glm::vec4& color);

private:
	virtual void GetAllUniforms()			override;
	virtual void SetPermanentAttributes()	override;

private:
	bool SetMatrixData(Transform* transform);
	void SetTextProperties(const glm::vec4& color);
};