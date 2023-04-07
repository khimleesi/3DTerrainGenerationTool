#include <pretty_freetype/ft2build.h>
#include <pretty_freetype/freetype.h>

#include <vector>
#include <map>
#include "Text.h"
#include "utilities/Log.h"
#include "managers/ResourceManager.h"
#include "graphics/shaders/TextShader.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Text::Text(const std::string& font, unsigned int size)
	:	m_tag(font),
		m_transform(glm::vec2(0.0f), glm::vec2(1.0f)),
		m_color(glm::vec4(1.0f))
{
	Load(size);
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Text::~Text()
{

}


/*******************************************************************************************************************
	A function that loads in a new font using FreeType library
*******************************************************************************************************************/
bool Text::Load(unsigned int size)
{
	if (m_tag.empty()) { return false; }

	//--- If the font already exists in our font cache, just use the previous buffers and textures generated for this font
	if (Resource::Instance()->FindFont(m_tag)) {
		COG_LOG("[FONT] Font already exists for tag: ", m_tag.c_str(), LOG_RESOURCE);
		return false;
	}

	std::string src = "Assets\\Fonts\\" + m_tag;

	//--- Initialize FreeType, returns any number other than 0 on fail
	FT_Library freeType = { 0 };
	if (FT_Init_FreeType(&freeType))
	{
		COG_LOG("[FONT] Problem initializing FreeType", COG_LOG_EMPTY, LOG_ERROR);
		return false;
	}

	//--- Load the font file, returns any number other than 0 on fail
	FT_Face face = { 0 };
	if (FT_New_Face(freeType, src.c_str(), 0, &face))
	{
		COG_LOG("[FONT] Failed to load font: ", src.c_str(), LOG_ERROR);
		return false;
	}

	//--- Set the dimensions of the font
	//--- Width should remain at 0 for automatic determination of font dimensions
	//--- The size we pass in should be quite a large number, this will make our font always render in a high resolution
	//--- And we can scale it down later (always better to scale down, than up!)
	FT_Set_Pixel_Sizes(face, 0, size);

	//--- Disable byte alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	//--- Create a temporary map for the font characters
	std::map<GLchar, FontCache::Character> characters;
	
	//--- For every character in this font (default 128), generate a glyph
	for (GLubyte glyph = 0; glyph < FontCache::Character::s_maxGlyphs; glyph++)
	{
		//--- Load character glyph 
		if (FT_Load_Char(face, glyph, FT_LOAD_RENDER))
		{
			COG_LOG("[FONT] Failed to load glyph: ", glyph, LOG_ERROR);
			continue;
		}

		//--- Generate texture for each glyph
		GLuint textureID = 0;
		COG_GLCALL(glGenTextures(1, &textureID));
		COG_GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));
		
		unsigned int width		= face->glyph->bitmap.width;
		unsigned int height		= face->glyph->bitmap.rows;
		unsigned char* pixels	= face->glyph->bitmap.buffer;

		//--- Use GL_RED for fonts
		COG_GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels));

		COG_GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		COG_GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		COG_GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		COG_GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		FT_Int leftOffset	= face->glyph->bitmap_left;
		FT_Int topOffset	= face->glyph->bitmap_top;
		FT_Pos advance		= face->glyph->advance.x;
		
		//--- Now store this character for later use
		FontCache::Character character = { textureID, { width, height }, { leftOffset, topOffset }, advance };

		characters.emplace(glyph, character);

		COG_LOG("[FONT] Generated glyph, texture ID: ", textureID, LOG_MEMORY);
	}

	COG_GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

	//--- Destroy the font and remove the FreeType object now that OpenGL has the data
	FT_Done_Face(face);
	FT_Done_FreeType(freeType);

	//--- Add this new font to our font cache
	Resource::Instance()->AddFont(m_tag, characters);

	COG_LOG("[FONT] Generated font: ", m_tag.c_str(), LOG_MEMORY);
	
	//--- Now set up the OpenGL buffers for this new font
	SetupBuffers();

	return true;
}


/*******************************************************************************************************************
	A function that sets up the OpenGL buffers for a new font
*******************************************************************************************************************/
void Text::SetupBuffers()
{
	std::vector<GLfloat> textureCoords = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	std::vector<GLfloat> vertices(s_numVertices);

	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Push(vertices, VertexBuffer::LAYOUT_POSITION, true);
		Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_UV)->Push(textureCoords, VertexBuffer::LAYOUT_UV, false);
	Resource::Instance()->GetVAO(m_tag)->Unbind();
}


/*******************************************************************************************************************
	A function that renders the font to the screen (notice we take in the shader - font is rendered differently)
*******************************************************************************************************************/
void Text::Render(Shader* shader, const std::string& text, const Transform& transform, const glm::vec4& color)
{
	//--- Change the colour for this string
	m_color = color;

	//--- We have to change the transformation for every string we render (not every text object)
	m_transform = transform;

	//--- And update the shader for every string we render
	if (TextShader* textShader = Downcast<TextShader>(shader)) {
		
		textShader->SetInstanceData(&m_transform, m_color);
	}

	COG_GLCALL(glActiveTexture(Shader::GetTextureUnit(Shader::TEXTURE_TEXT)));

	Resource::Instance()->GetVAO(m_tag)->Bind();

	//--- Go through every character within this string
		for (auto& c : text) {

			//--- And get the character from the font cache
			FontCache::Character* character = Resource::Instance()->GetFontCharacter(m_tag, c);
	
			if (character) {

				//--- Set up our new position and dimension of this character
				glm::vec2 position(	m_transform.GetPosition().x + character->bearing.x * m_transform.GetDimensions().x,
									m_transform.GetPosition().y - (character->size.y - character->bearing.y) * m_transform.GetDimensions().y);

				glm::vec2 dimension(character->size.x * m_transform.GetDimensions().x,
									character->size.y * m_transform.GetDimensions().y);

				std::vector<GLfloat> vertices = {
					position.x,					position.y + dimension.y,	0.0f,
					position.x,					position.y,					0.0f,
					position.x + dimension.x,	position.y,					0.0f,
					position.x,					position.y + dimension.y,	0.0f,
					position.x + dimension.x,	position.y,					0.0f,
					position.x + dimension.x,	position.y + dimension.y,	0.0f
				};

				//--- Bind this characters texture ID
				COG_GLCALL(glBindTexture(GL_TEXTURE_2D, character->ID));

				//--- And update the OpenGL buffers with the new positions for this character and render to the screen
				Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Update(vertices);
				Resource::Instance()->GetVBO(m_tag, VertexBuffer::LAYOUT_POSITION)->Render();

				//--- Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				m_transform.SetPosition(m_transform.GetPosition().x + (character->advance >> 6) * m_transform.GetDimensions().x, m_transform.GetPosition().y); // Bitshift by 6 to get value in pixels (2^6 = 64)
			}
		}

	//--- Make sure we unbind everything when we are done rendering this string
	Resource::Instance()->GetVAO(m_tag)->Unbind();

	COG_GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec4& Text::GetColor() const { return m_color; }
Transform* Text::GetTransform()			{ return &m_transform; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const unsigned int Text::s_numVertices = 18;