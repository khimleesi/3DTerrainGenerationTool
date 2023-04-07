#pragma once

/*******************************************************************************************************************
	Texture.h, Texture.cpp
	Created by Kim Kane
	Last updated: 21/03/2018
	Class finalized: 02/04/2018

	Generates a 2D texture using the SDL library.

	[Features]
	Integrated with OpenGL to allow generic use of different texture slots upon texture creation.
	Textures only get created once - all textures with the same name will re-use textures already loaded.
	(See TextureCache and ResourceManager to see how this works)
	Transparent textures supported.
	Fake lighting supported.
	Texture atlases supported.
	Texture mirroring supported.
	Cube maps supported.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	None at present.

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <pretty_sdl/SDL_image.h>
#include <string>
#include <vector>
#include "managers/FileManager.h"

class Texture {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(
			COG_NVP(m_tag),
			COG_NVP(m_width),
			COG_NVP(m_height),
			COG_NVP(m_index),
			COG_NVP(m_rows),
			COG_NVP(m_offset),
			COG_NVP(m_hasTransparency),
			COG_NVP(m_hasFakeLighting),
			COG_NVP(m_isMirrored));
	}


private:
	struct OpenGLTextureData {
		GLuint	ID;
		int		slot;
		int		type;
	};

public:
	Texture(const std::string& texture, int slot, bool hasTransparency = false, bool hasFakeLighting = false);
	Texture(const std::string& tag, const std::vector<std::string>& textures, int slot);
	Texture(int attachment, int width, int height, int slot, const std::string& tag);
	Texture();
	~Texture();

	bool LoadTexture(const std::string& texture, int slot, bool hasTransparency = false, bool hasFakeLighting = false);
	bool LoadSkyboxTextures(const std::string& tag, const std::vector<std::string>& textures, int slot);
	bool LoadRenderTargetTexture(int attachment, int width, int height, int slot, const std::string& tag);

public:
	void Bind() const;
	void Unbind() const;

public:
	const OpenGLTextureData* GetData() const;
	const std::string& GetTag() const;

public:
	int GetWidth() const;
	int GetHeight() const;
	int GetRows() const;
	const glm::vec2& GetOffset();
	static unsigned int GetDefaultRows();

public:
	bool HasTransparency() const;
	bool HasFakeLighting() const;
	bool IsMirrored() const;

public:
	void SetIndex(int index);
	void SetRows(int rows);

public:
	void SetFakeLighting(bool hasFakeLighting);
	void SetTransparency(bool hasTransparency);
	void SetMirrored(bool isMirrored);

private:
	bool Load();
	bool Load(const std::vector<std::string>& textures);
	bool Load(int attachment);

private:
	void GenerateTexture();

private:
	std::string			m_tag;
	OpenGLTextureData	m_data;
	int					m_width, m_height;
	int					m_index, m_rows;
	glm::vec2			m_offset;

private:
	bool m_hasTransparency;
	bool m_hasFakeLighting;
	bool m_isMirrored;

private:
	static unsigned int s_defaultRows;
	static unsigned int s_defaultIndex;
};