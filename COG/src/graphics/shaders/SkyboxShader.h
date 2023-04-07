#pragma once

/*******************************************************************************************************************
	SkyboxShader.h, SkyboxShader.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A derived class of base class Shader. The main shader used for the in-game skybox.

	[Features]
	Supports camera rotation.
	Skybox sampler is an OpenGL cubemap.
	Has tinting effects for when you want to merge the skybox with environment fog.
	Shader data only get's updated when changes have happened.

	[Upcoming]
	Reflection mapping.
	Option of day/night cycle.
	Minor rotation effects/moving clouds to make skybox appear more realistic.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "Shader.h"

class SkyboxShader : public Shader {

public:
	SkyboxShader(const std::string& vertex, const std::string& fragment, Camera* camera);
	virtual ~SkyboxShader();

public:
	void SetInstanceData();
	virtual void SetSkyboxData(bool isTintEnabled, float tintBegin, float tintEnd, const glm::vec3& tintColor) override;

private:
	virtual void GetAllUniforms()			override;
	virtual void SetPermanentAttributes()	override;

private:
	bool SetMatrixData();

private:
	glm::mat4 m_projection;
};