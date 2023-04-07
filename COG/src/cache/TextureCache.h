#pragma once

/*******************************************************************************************************************
	TextureCache.h, TextureCache.cpp
	Created by Kim Kane
	Last updated: 06/04/2018

	Handles the correct memory storage for all our in-game OpenGL texture ID's.

	[Features]
	Adds all OpenGL texture ID's to a texture cache, permitting re-use of textures already in memory.
	Destroy's all texture ID's upon Unload function being called.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	All cache classes should be created within either a static class or a singleton.
	They should only be created within a permanent location already in memory, as their lifetime
	is meant to last throughout the duration of the program.
	If you want multiple instances - as you might want more than one cache for textures, etc. just remove
	the static keyword

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <map>
#include <string>

class TextureCache {

public:
	TextureCache();

public:
	void Unload();
	~TextureCache();

public:
	void AddTexture(const std::string& tag, GLuint id);
	bool FindTexture(const std::string& tag);

public:
	const GLuint& GetTexture(const std::string& tag);

private:
	typedef std::map <std::string, GLuint> Cache;

private:
	static Cache s_textures;
};