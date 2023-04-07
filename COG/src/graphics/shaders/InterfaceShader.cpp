#include "InterfaceShader.h"
#include "physics/Transform.h"
#include "graphics/Texture.h"
#include "managers/ScreenManager.h"
#include "utilities/Log.h"
#include "utilities/Tools.h"
#include "application/Interface.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/ 
InterfaceShader::InterfaceShader(const std::string& vertex, const std::string& fragment)
	:	Shader(vertex, fragment)
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
InterfaceShader::~InterfaceShader()
{

}


/*******************************************************************************************************************
	A function which get's all uniform locations within the shader
*******************************************************************************************************************/
void InterfaceShader::GetAllUniforms()
{
	GetUniform("uniform_interface_projection");
	GetUniform("uniform_interface_texture");
	GetUniform("uniform_interface_textureData.offset");
	GetUniform("uniform_interface_textureData.rows");
	GetUniform("uniform_interface_textureData.isMirrored");
}


/*******************************************************************************************************************
	A function that set's all the unchanging data within the shader
*******************************************************************************************************************/
void InterfaceShader::SetPermanentAttributes()
{
	SetInteger("uniform_interface_texture", TEXTURE_INTERFACE);
}


/*******************************************************************************************************************
	A function that set's the dynamic data within the shader, relative to an inteface object
*******************************************************************************************************************/
void InterfaceShader::SetInstanceData(Transform* transform, Texture* texture)
{
	//--- Check we have a valid program
	if (m_shaderCount != NULL) {
		SetMatrixData(transform);
		SetTextureData(texture);
	}
}


/*******************************************************************************************************************
	A function that set's the projection of the interface objects (needs to be done for every object's transform)
*******************************************************************************************************************/
bool InterfaceShader::SetMatrixData(Transform* transform)
{
	if (!transform) { return false; }

	//--- Get the projection matrix and multiply this with the transform matrix of the 2D object
	glm::mat4 projection = Screen::Instance()->GetProjectionMatrix() * transform->GetTransformationMatrix();

	SetMatrix("uniform_interface_projection", projection);

	return true;
}


/*******************************************************************************************************************
	A function that set's all the texture data within the shader (needs to be done for every object's texture data)
*******************************************************************************************************************/
bool InterfaceShader::SetTextureData(Texture* texture)
{
	if (!texture) { return false; }

	SetVector2f("uniform_interface_textureData.offset", texture->GetOffset());
	SetFloat("uniform_interface_textureData.rows", (float)texture->GetRows());
	SetBool("uniform_interface_textureData.isMirrored", (int)texture->IsMirrored());

	return true;
}