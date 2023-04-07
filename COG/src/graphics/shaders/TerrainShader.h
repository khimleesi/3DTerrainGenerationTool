#pragma once

/*******************************************************************************************************************
	TerrainShader.h, TerrainShader.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A derived class of base class Shader. The main shader used for the in-game terrain.

	[Features]
	Supports multi-texturing and multi-normal mapping.
	Supports an array of spot lights, point lights and directional lights.
	Additional support for fog effects.
	Uses singular uniforms and uniform blocks to create and update shader data.
	Shader data only get's updated when changes have happened.

	[Upcoming]
	Ground fog.
	Generic amount of blend maps/textures/normal maps.
	Frame buffer support to save the entire terrain to an image and then generate
	the normal map from that image (If that's even possible!)
	Mini-maps.
	Much, much more...

	[Side Notes]
	I think it's pretty :)

*******************************************************************************************************************/
#include "UniformBlocks.h"

class TerrainShader : public Shader {

public:
	TerrainShader(const std::string& vertex, const std::string& fragment, Camera* camera);
	virtual ~TerrainShader();

public:
	void SetInstanceData(Transform* transform, Texture* texture, bool minimapMode);
	virtual bool SetLights(const std::vector<Light*>& lights) override;
	virtual void DebugMode(bool enableDebugSettings) override;
	virtual void SetFogData(int type, bool rangeBased, float density, const glm::vec4& color) override;

private:
	virtual void GetAllUniforms()			override;
	virtual void SetPermanentAttributes()	override;

private:
	bool SetMatrixData(Transform* transform);
	bool SetTextureData(Texture* texture);
	void SetMinimapMode(bool minimapMode);

private:
	uniform_block::MatrixData	m_matrixData;
	uniform_block::FogData		m_fogData;
	uniform_block::LightData	m_lightData;

private:
	bool m_isMirrored;
};