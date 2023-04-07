#pragma once

/*******************************************************************************************************************
	EntityShader.h, EntityShader.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A derived class of base class Shader. The main shader used for all 3D game objects.

	[Features]
	Supports various materials - diffuse, specular, emissive and normal maps.
	Supports texture atlases, texture transparency and texture mirroring.
	Supports an array of spot lights, point lights and directional lights.
	Additional support for fog effects.
	Uses singular uniforms and uniform blocks to create and update shader data.
	Shader data only get's updated when changes have happened.

	[Upcoming]
	Deferred rendering support and frame buffer support.
	Batch rendering system to improve performance and make switching textures/materials much faster and more optimized.
	PBR, parallax mapping, shadow mapping, bloom.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "UniformBlocks.h"

class EntityShader : public Shader {

public:
	EntityShader(const std::string& vertex, const std::string& fragment, Camera* camera);
	virtual ~EntityShader();
	
public:
	void SetInstanceData(Transform* transform, Material* material);
	virtual bool SetLights(const std::vector<Light*>& lights) override;
	virtual void DebugMode(bool enableDebugSettings) override;

private:
	virtual void GetAllUniforms()			override;
	virtual void SetPermanentAttributes()	override;

private:
	bool SetMatrixData(Transform* transform);
	void SetFogData(int type, bool rangeBased, float density, const glm::vec4& color);
	bool SetTextureData(Texture* texture);
	bool SetMaterialData(Material* material);

private:
	uniform_block::MatrixData	m_matrixData;
	uniform_block::FogData		m_fogData;
	uniform_block::LightData	m_lightData;
	uniform_block::TextureData	m_textureData;
	uniform_block::MaterialData m_materialData;
};