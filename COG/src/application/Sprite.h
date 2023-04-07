#pragma once
/*******************************************************************************************************************
	Sprite.h, Sprite.cpp
	Created by Kim Kane
	Last updated: 12/04/2018

	A sprite is a 2D quad with a texture.
	
	[Features]
	Supports animations using texture atlases.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <string>
#include "graphics/Texture.h"
#include "application/Quad.h"

class Sprite {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_texture), COG_NVP(m_quad), COG_NVP(m_isAnimated));
	}

public:
	Sprite(	const std::string& tag, const std::string& texture,
			bool isAnimated = false, unsigned int rows = Texture::GetDefaultRows());
	Sprite();

	bool Create(const std::string& tag, const std::string& texture,
				bool isAnimated = false, unsigned int rows = Texture::GetDefaultRows());
	
	bool Load(const std::string& tag);

public:
	~Sprite();

public:
	void Render();
	void Update(float frame);

public:
	Texture* GetTexture();
	bool IsAnimated() const;

public:
	void SetAnimated(bool isAnimated, unsigned int rows);
	void SetAnimationFrame(float frame);

private:
	Texture		m_texture;
	Quad		m_quad;

private:
	bool		m_isAnimated;
};