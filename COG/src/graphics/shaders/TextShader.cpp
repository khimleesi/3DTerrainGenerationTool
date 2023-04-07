#include "TextShader.h"
#include "physics/Transform.h"
#include "managers/ScreenManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
TextShader::TextShader(const std::string& vertex, const std::string& fragment)
	: Shader(vertex, fragment)
{
	//--- Check we have a valid program
	if (m_shaderCount != NULL) {

		//--- Get all uniforms within the shader, when an instance of this shader is first created
		GetAllUniforms();
		
		//--- Set all unchanging data within this shader
		Bind(); SetPermanentAttributes();
	}
}


/*******************************************************************************************************************
	Default Destructor
*******************************************************************************************************************/
TextShader::~TextShader()
{

}


/*******************************************************************************************************************
	A function which get's all uniform locations within the shader
*******************************************************************************************************************/
void TextShader::GetAllUniforms()
{
	GetUniform("uniform_text_projection");
	GetUniform("uniform_text_texture");
	GetUniform("uniform_text_textColor");
}


/*******************************************************************************************************************
	A function that set's all the unchanging data within the shader
*******************************************************************************************************************/
void TextShader::SetPermanentAttributes()
{
	SetInteger("uniform_text_texture", TEXTURE_TEXT);
}


/*******************************************************************************************************************
	A function that set's all the per string data (not per instance) within the shader (specific to this game)
*******************************************************************************************************************/
void TextShader::SetInstanceData(Transform* transform, const glm::vec4& color)
{
	//--- Check we have a valid program
	if (m_shaderCount != NULL) {
		SetMatrixData(transform);
		SetTextProperties(color);
	}
}


/*******************************************************************************************************************
	A function that set's the projection of the text string (has to be done for every string we render)
*******************************************************************************************************************/
bool TextShader::SetMatrixData(Transform* transform)
{
	if (!transform) { return false; }

	//--- Get the projection matrix and multiply this with the transform matrix of the text string
	glm::mat4 projection = Screen::Instance()->GetProjectionMatrix() * transform->GetTransformationMatrix();

	//--- Update shader for every text string we render
	SetMatrix("uniform_text_projection", projection);

	return true;
}


/*******************************************************************************************************************
	A function that set's text properties in the shader
*******************************************************************************************************************/
void TextShader::SetTextProperties(const glm::vec4& color)
{
	SetVector4f("uniform_text_textColor", color);
}