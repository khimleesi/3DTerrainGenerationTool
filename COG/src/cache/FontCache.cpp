#include "FontCache.h"
#include "utilities/Tools.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
FontCache::FontCache()
{

}


/*******************************************************************************************************************
	Default Destructor
*******************************************************************************************************************/
FontCache::~FontCache()
{

}


/*******************************************************************************************************************
	Function that unloads all fonts from memory and destroys the font cache
*******************************************************************************************************************/
void FontCache::Unload() {

	static unsigned int fontCount = s_fonts.size();

	COG_LOG("[FONT CACHE] s_fonts map size before deletion: ", fontCount, LOG_RESOURCE);

	for (auto& font : s_fonts)
	{
		//--- For this font, get all the characters and delete them
		for (GLubyte glyph = 0; glyph < Character::s_maxGlyphs; glyph++)
		{
			COG_LOG("[FONT CACHE] Deleting glyph texture from s_fonts map: "
				+ GetKey(font) + ", OpenGL texture ID: ", GetValue(font).at(glyph).ID, LOG_MEMORY);

			COG_GLCALL(glDeleteTextures(1, &GetValue(font).at(glyph).ID));
		}

		COG_LOG("[FONT CACHE] Font removed: ", GetKey(font).c_str(), LOG_RESOURCE);

		fontCount--;

		//--- Explicity clear the map before the program ends so all default destructor's are called correctly
		if (fontCount == 0) { s_fonts.clear(); break; }
	}

	COG_LOG("[FONT CACHE] s_fonts map size after deletion: ", fontCount, LOG_RESOURCE);
}


/*******************************************************************************************************************
	Function that adds a font to the font map if it doesn't already exist
*******************************************************************************************************************/
void FontCache::AddFont(const std::string& tag, const std::map<GLchar, Character>& characters)
{
	s_fonts.try_emplace(tag, characters);

	COG_LOG("[FONT CACHE] Font added to s_fonts map: ", tag.c_str(), LOG_RESOURCE);
}


/*******************************************************************************************************************
	Function that checks if a font exists in the font map, returns true if so
*******************************************************************************************************************/
bool FontCache::FindFont(const std::string& tag)
{
	auto font = s_fonts.find(tag);

	return font != s_fonts.end();
}


/*******************************************************************************************************************
	Function that get's a single character bound to a font in memory
*******************************************************************************************************************/
FontCache::Character* FontCache::GetCharacter(const std::string& tag, GLchar character) { return &s_fonts.at(tag).at(character); }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
GLubyte FontCache::Character::s_maxGlyphs	= 128;
FontCache::Cache FontCache::s_fonts;