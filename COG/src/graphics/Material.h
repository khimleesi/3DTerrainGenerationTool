#pragma once

/*******************************************************************************************************************
	Material.h, Material.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	A material class that encapsulates diffuse, specular, emissive and normal map textures.

	[Features]
	Supports diffuse, specular, emissive and normal map textures.

	[Upcoming]
	Tint color to alter the final texture color.
	Parallax maps.

	[Side Notes]
	None at present.

*******************************************************************************************************************/
#include <map>
#include <string>
#include "graphics/Texture.h"
#include "graphics/shaders/Shader.h"

class Material {

public:
	Material(const std::string& diffuse, const std::string& normal = "", const std::string& specular = "", const std::string& emissive = "");
	~Material();

public:
	void Bind();
	void Unbind();

public:
	float GetShininess() const;

public:
	Texture* GetDiffuse();
	Texture* GetEmissive();
	Texture* GetSpecular();
	Texture* GetNormalMap();

public:
	bool IsReflective() const;
	bool IsGlowing() const;
	bool IsNormalMapped() const;

public:
	void SetShininess(float shininess);

private:
	void Load(const std::string& diffuse, const std::string& normal, const std::string& specular, const std::string& emissive);
	bool AddTexture(Shader::TextureUnit unit, const std::string& texture);

private:
	float m_shininess;
	
private:
	bool m_isReflective;
	bool m_isGlowing;
	bool m_isNormalMapped;

private:
	std::map<Shader::TextureUnit, Texture> m_textures;

private:
	static float s_defaultShininess;
};