#pragma once

/*******************************************************************************************************************
	TexturePack.h, TexturePack.cpp
	Created by Kim Kane
	Last updated: 02/04/2018

	A simple class that loads in multiple textures at once and stores them into an array.
	Prime usage would be for terrain multi-textures.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	Not much to say here!

*******************************************************************************************************************/

#include <array>
#include <map>
#include <string>
#include "graphics/Texture.h"
#include "graphics/shaders/Shader.h"

class TexturePack {
	
	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_textures));
	}

public:
	TexturePack(const std::string& baseTexture,
				const std::string& rTexture,
				const std::string& gTexture,
				const std::string& bTexture,
				const std::string& blendMap);

	TexturePack(const std::string& baseTextureNormal,
				const std::string& rTextureNormal,
				const std::string& gTextureNormal,
				const std::string& bTextureNormal);
public:
	TexturePack() {}
	~TexturePack();

public:
	void LoadDiffuse(const std::string& baseTexture,
		const std::string& rTexture,
		const std::string& gTexture,
		const std::string& bTexture,
		const std::string& blendMap);

	void LoadNormal(const std::string& baseTextureNormal,
		const std::string& rTextureNormal,
		const std::string& gTextureNormal,
		const std::string& bTextureNormal);

	void LoadDiffuseFromMap();
	void LoadNormalFromMap();

public:
	void Bind() const;
	void Unbind() const;

public:
	Texture* GetBlendMap();
	std::map<Shader::TextureUnit, Texture>* GetTextures() { return &m_textures; }

private:
	void Load(const std::string& baseTexture,
			  const std::string& rTexture,
			  const std::string& gTexture,
			  const std::string& bTexture,
			  const std::string& blendMap);

	void Load(const std::string& baseTextureNormal,
			  const std::string& rTextureNormal,
			  const std::string& gTextureNormal,
			  const std::string& bTextureNormal);

private:
	bool AddTexture(Shader::TextureUnit unit, const std::string& texture);

private:
	std::map<Shader::TextureUnit, Texture> m_textures;

};