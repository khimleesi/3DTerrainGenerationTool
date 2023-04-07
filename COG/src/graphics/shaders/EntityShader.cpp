#include "EntityShader.h"
#include "utilities/Log.h"
#include "managers/ScreenManager.h"
#include "graphics/Camera.h"
#include "physics/Transform.h"
#include "graphics/Texture.h"
#include "graphics/Material.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
EntityShader::EntityShader(const std::string& vertex, const std::string& fragment, Camera* camera)
	:	Shader(vertex, fragment, camera)
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
EntityShader::~EntityShader()
{

}


/*******************************************************************************************************************
	A function which get's all uniform locations within the shader
*******************************************************************************************************************/
void EntityShader::GetAllUniforms()
{
	//--- Get the samplers
	GetUniform("uniform_entity_material.diffuse");
	GetUniform("uniform_entity_material.specular");
	GetUniform("uniform_entity_material.emission");
	GetUniform("uniform_entity_material.normal");

	//--- Get the uniform blocks and pass in the size of the data we will be sending and the binding location
	GetUniformBlock("uniform_block_entity_matrixData", sizeof(uniform_block::MatrixData), BIND_ENTITY_MATRIX_DATA);
	GetUniformBlock("uniform_block_entity_textureData", sizeof(uniform_block::TextureData), BIND_ENTITY_TEXTURE_DATA);
	GetUniformBlock("uniform_block_entity_fogData", sizeof(uniform_block::FogData), BIND_ENTITY_FOG_DATA);
	GetUniformBlock("uniform_block_entity_lightData", sizeof(uniform_block::LightData), BIND_ENTITY_LIGHT_DATA);
	GetUniformBlock("uniform_block_entity_materialData", sizeof(uniform_block::MaterialData), BIND_ENTITY_MATERIAL_DATA);

	//--- DEBUG TOOLS
	GetUniform("uniform_entity_debugMode");
}


/*******************************************************************************************************************
	A function that toggles debug mode on/off
*******************************************************************************************************************/
void EntityShader::DebugMode(bool enableDebugSettings)
{
	SetBool("uniform_entity_debugMode", enableDebugSettings);
}


/*******************************************************************************************************************
	A function that set's all the unchanging data within the shader (specific to this game)
*******************************************************************************************************************/
void EntityShader::SetPermanentAttributes()
{
	using namespace shader_constants;

	//--- Set the fog data (unchanging in this game, but can be dynamic as well)
	SetFogData(FOG_EXP, IS_FOG_RANGED, FOG_DENSITY, FOG_COLOR);

	//--- Set the entity samplers once as these never change
	SetInteger("uniform_entity_material.diffuse", TEXTURE_DIFFUSE);
	SetInteger("uniform_entity_material.specular", TEXTURE_SPECULAR);
	SetInteger("uniform_entity_material.emission", TEXTURE_EMISSIVE);
	SetInteger("uniform_entity_material.normal", TEXTURE_NORMAL);
}


/*******************************************************************************************************************
	A function that set's all the per instance entity data within the shader (specific to this game)
*******************************************************************************************************************/
void EntityShader::SetInstanceData(Transform* transform, Material* material)
{
	if (m_shaderCount != NULL) {
		SetMatrixData(transform);
		SetMaterialData(material);
	}
}


/*******************************************************************************************************************
	A function that set's the matrix data within the shader (should only be done when a change happens)
*******************************************************************************************************************/
bool EntityShader::SetMatrixData(Transform* transform)
{
	if (!m_camera || !transform) { return false; }

	bool hasChanged = false;

	glm::mat4 projection	= Screen::Instance()->GetProjectionMatrix();
	glm::mat4 view			= m_camera->GetViewMatrix();
	glm::mat4 world			= transform->GetTransformationMatrix();
	glm::mat4 intraWorld	= glm::transpose(glm::inverse(world));

	//--- Check if any data has changed and only update the old data if so
	if (m_matrixData.projection != projection)	{ m_matrixData.projection = projection; hasChanged = true; }
	if (m_matrixData.view != view)				{ m_matrixData.view = view; hasChanged = true; }
	if (m_matrixData.world != world)			{ m_matrixData.world = world; hasChanged = true; }
	if (m_matrixData.intraWorld != intraWorld)	{ m_matrixData.intraWorld = intraWorld; hasChanged = true; }

	//--- And only update the shader if any changes have happened
	if (hasChanged) { GetBinding(BIND_ENTITY_MATRIX_DATA)->Update(&m_matrixData); }

	return true;
}


/*******************************************************************************************************************
	A function that set's the fog data within the shader (should only be done once, unless changing)
*******************************************************************************************************************/
void EntityShader::SetFogData(int type, bool rangeBased, float density, const glm::vec4& color)
{
	//--- *Fog variable stored in permanent memory in-case we want to change the fog at any point during the game

	bool hasChanged = false;
		
	//--- Check if any data has changed and only update the old data if so
	if (m_fogData.fogColor != color)				{ m_fogData.fogColor = color; hasChanged = true; }
	if (m_fogData.fogType != type)					{ m_fogData.fogType = type; hasChanged = true; }
	if (m_fogData.rangeBasedFog != (int)rangeBased)	{ m_fogData.rangeBasedFog = (int)rangeBased; hasChanged = true; }
	if (m_fogData.fogDensity != density)			{ m_fogData.fogDensity = density; hasChanged = true; }

	//--- Update the shader if data has changed
	if (hasChanged) { GetBinding(BIND_ENTITY_FOG_DATA)->Update(&m_fogData); }
}


/*******************************************************************************************************************
	A function that set's all the light data within the shader (should only be done once, unless changing)
*******************************************************************************************************************/
bool EntityShader::SetLights(const std::vector<Light*>& lights)
{
	if (lights.empty() || !m_camera) { return false; }

	//--- Make sure the lights vector size is no more than our max lights set in the shader
	if (lights.size() > MAX_LIGHTS) {
		COG_LOG("[ENTITY SHADER] Max lights limit reached: ", lights.size(), LOG_ERROR);
		return false;
	}

	//--- Make sure we have a valid program
	if (m_shaderCount != NULL) {

		bool hasChanged = false;

		glm::vec4 cameraPosition = glm::vec4(m_camera->GetPosition(), 1.0f);

		//--- Check if the data has changed and only update the old data if so
		if (m_lightData.numLights != lights.size())		{ m_lightData.numLights = lights.size(); hasChanged = true; }
		if (m_lightData.eyePosition != cameraPosition)	{ m_lightData.eyePosition = cameraPosition; hasChanged = true; }

		for (int i = 0; i < m_lightData.numLights; i++) {

			//--- If both lights are the same, ignore them
			if (m_lightData.lights[i] == *lights[i]) { continue; }

			//--- If a light has changed we update the data
			else { m_lightData.lights[i] = *lights[i]; hasChanged = true; }
		}

		//--- Only update shader with data if a change has happened
		if (hasChanged) { GetBinding(BIND_ENTITY_LIGHT_DATA)->Update(&m_lightData); }
	}

	return true;
}


/*******************************************************************************************************************
	A function that set's all the material data within the shader (needs to be done for every object's material)
*******************************************************************************************************************/
bool EntityShader::SetMaterialData(Material* material)
{
	if (!material) { return false; }

	m_materialData.isReflective		= (int)material->IsReflective();
	m_materialData.isGlowing		= (int)material->IsGlowing();
	m_materialData.isNormalMapped	= (int)material->IsNormalMapped();
	m_materialData.shininess		= material->GetShininess();

	//--- Update the shader with this object's data
	GetBinding(BIND_ENTITY_MATERIAL_DATA)->Update(&m_materialData);

	//--- Update the material's diffuse texture only
	SetTextureData(material->GetDiffuse());

	return true;
}


/*******************************************************************************************************************
	A function that set's all the texture data within the shader (needs to be done for every object's texture data)
*******************************************************************************************************************/
bool EntityShader::SetTextureData(Texture* texture)
{
	if (!texture) { return false; }

	m_textureData.offset			= texture->GetOffset();
	m_textureData.rows				= (float)texture->GetRows();
	m_textureData.hasFakeLighting	= (int)texture->HasFakeLighting();
	m_textureData.isMirrored		= (int)texture->IsMirrored();

	//--- Update the shader
	GetBinding(BIND_ENTITY_TEXTURE_DATA)->Update(&m_textureData);

	return true;
}