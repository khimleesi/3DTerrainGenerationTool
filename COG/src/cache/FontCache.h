#pragma once

/*******************************************************************************************************************
	FontCache.h, FontCache.cpp
	Created by Kim Kane
	Last updated: 06/04/2018

	Handles the correct memory storage for all our in-game fonts.

	[Features]
	Supports font's that are loaded in via the FreeType font library.
	Supports caching of fonts and emplaces them into a font cache.
	Re-uses existing fonts already in the cache.
	Access to individual characters of a font.

	[Upcoming]
	Multi-support for bitmap fonts also.
	Conversion of FreeType font into just one bitmap, rather than multiple textures per character.

	[Side Notes]
	All cache classes should be created within either a static class or a singleton.
	They should only be created within a permanent location already in memory, as their lifetime
	is meant to last throughout the duration of the program.
	If you want multiple instances - as you might want more than one cache for fonts, just remove
	the static keyword

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <string>
#include <map>

class FontCache { 

public:
	struct Character {
		GLuint			ID;
		glm::ivec2		size;
		glm::ivec2		bearing;
		signed long		advance;
		static GLubyte	s_maxGlyphs;
	};

public:
	FontCache();

public:
	void Unload();
	~FontCache();

public:
	void AddFont(const std::string& tag, const std::map<GLchar, Character>& characters);
	bool FindFont(const std::string& tag);

public:
	Character* GetCharacter(const std::string& tag, GLchar character);

private:
	typedef std::map<std::string, std::map<GLchar, Character>> Cache;

private:
	static Cache s_fonts;
};