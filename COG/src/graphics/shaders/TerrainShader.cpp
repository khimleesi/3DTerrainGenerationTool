#include "TerrainShader.h"
#include "managers/ScreenManager.h"
#include "utilities/Log.h"
#include "graphics/Camera.h"
#include "physics/Transform.h"
#include "graphics/Texture.h"
#include "application/Terrain.h"

/*******************************************************************************************************************
	Default Constructor
*******************************************************************************************************************/
TerrainShader::TerrainShader(const std::string& vertex, const std::string& fragment, Camera* camera)
	:	Shader(vertex, fragment, camera),
		m_isMirrored(false)
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
TerrainShader::~TerrainShader()
{

}


/*******************************************************************************************************************
	A function which get's all uniform locations within the shader
*******************************************************************************************************************/
void TerrainShader::GetAllUniforms()
{
	//--- Get all the terrain samplers
	for (unsigned int i = 0; i < Terrain::GetMaxTextures(); i++) {
		GetUniform("uniform_terrain_textures[" + std::to_string(i) + "]");
	}
		
	for (unsigned int i = 0; i < Terrain::GetMaxNormalMaps(); i++) {
		GetUniform("uniform_terrain_normalMaps[" + std::to_string(i) + "]");
	}

	//--- The singular uniform that allows us to flip a texture
	GetUniform("uniform_terrain_isMirrored");

	//--- The singular uniform that allows us to switch minimap mode on/off
	GetUniform("uniform_terrain_minimapMode");

	//--- Get the uniform blocks and pass in the size of the data we will be sending and the binding location
	GetUniformBlock("uniform_block_terrain_matrixData", sizeof(uniform_block::MatrixData), BIND_TERRAIN_MATRIX_DATA);
	GetUniformBlock("uniform_block_terrain_fogData", sizeof(uniform_block::FogData), BIND_TERRAIN_FOG_DATA);
	GetUniformBlock("uniform_block_terrain_lightData", sizeof(uniform_block::LightData), BIND_TERRAIN_LIGHT_DATA);

	//--- DEBUG TOOLS
	GetUniform("uniform_terrain_debugMode");
}


/*******************************************************************************************************************
	A function that toggles debug mode on/off
*******************************************************************************************************************/
void TerrainShader::DebugMode(bool enableDebugSettings)
{
	SetBool("uniform_terrain_debugMode", enableDebugSettings);
}


/*******************************************************************************************************************
	A function that set's all the unchanging data within the shader, not relative to an instance
*******************************************************************************************************************/
void TerrainShader::SetPermanentAttributes()
{	
	using namespace shader_constants;

	//--- Set the fog data (unchanging in this game, but can be dynamic as well)
	SetFogData(FOG_EXP, IS_FOG_RANGED, FOG_DENSITY, FOG_COLOR);

	//--- Set the terrain samplers once as these never change
	for (unsigned int i = 0; i < Terrain::GetMaxTextures(); i++) {
		SetInteger("uniform_terrain_textures[" + std::to_string(i) + "]", TEXTURE_BASE + i);
	}

	for (unsigned int i = 0; i < Terrain::GetMaxNormalMaps(); i++) {
		SetInteger("uniform_terrain_normalMaps[" + std::to_string(i) + "]", TEXTURE_BASE_NORMAL + i);
	}
}


/*******************************************************************************************************************
	A function that set's all the data within the shader, relative to a terrain instance
*******************************************************************************************************************/
void TerrainShader::SetInstanceData(Transform* transform, Texture* texture, bool minimapMode)
{
	//--- Currently not much of the terrain data changes, but I wanted this to happen every frame
	//--- as in future I'd like to change things (e.g, texture effects)
	//--- However, the shader still won't be updated if no data changes
	if (m_shaderCount != NULL) {
		SetMatrixData(transform);
		SetTextureData(texture);
		SetMinimapMode(minimapMode);
	}
}


/*******************************************************************************************************************
	A function that set's the matrix data within the shader (should only be done when a change happens)
*******************************************************************************************************************/
bool TerrainShader::SetMatrixData(Transform* transform)
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
	if (hasChanged) {
		GetBinding(BIND_TERRAIN_MATRIX_DATA)->Update(&m_matrixData);
	}

	return true;
}


/*******************************************************************************************************************
	A function that set's the fog data within the shader (should only be done once, unless changing)
*******************************************************************************************************************/
void TerrainShader::SetFogData(int type, bool rangeBased, float density, const glm::vec4& color)
{
	//--- *Fog variable stored in permanent memory in-case we want to change the fog at any point during the game

	bool hasChanged = false;
		
	//--- Check if any data has changed and only update the old data if so
	if (m_fogData.fogColor != color)				{ m_fogData.fogColor = color; hasChanged = true; }
	if (m_fogData.fogType != type)					{ m_fogData.fogType = type; hasChanged = true; }
	if (m_fogData.rangeBasedFog != (int)rangeBased)	{ m_fogData.rangeBasedFog = (int)rangeBased; hasChanged = true; }
	if (m_fogData.fogDensity != density)			{ m_fogData.fogDensity = density; hasChanged = true; }

	//--- Update the shader if data has changed
	if (hasChanged) {
		GetBinding(BIND_TERRAIN_FOG_DATA)->Update(&m_fogData);
	}
}


/*******************************************************************************************************************
	A function that set's all the light data within the shader (should only be done once, unless changing)
*******************************************************************************************************************/
bool TerrainShader::SetLights(const std::vector<Light*>& lights)
{
	if (lights.empty() || !m_camera) { return false; }

	//--- Make sure the lights vector size is no more than our max lights set in the shader
	if (lights.size() > MAX_LIGHTS)
	{
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
		if (hasChanged) { 
			GetBinding(BIND_TERRAIN_LIGHT_DATA)->Update(&m_lightData);
		}
	}

	return true;
}


/*******************************************************************************************************************
	A function that set's the texture data within the shader (should only be done once, doesn't change)
*******************************************************************************************************************/
bool TerrainShader::SetTextureData(Texture* texture)
{
	if (!texture) { return false; }

	//--- If the terrain texture is not mirrored, this never needs to happen and so it will always be false
	if (m_isMirrored != texture->IsMirrored()) {
		m_isMirrored = texture->IsMirrored();
		SetBool("uniform_terrain_isMirrored", m_isMirrored);
	}

	return true;
}


/*******************************************************************************************************************
	A function that set's the minimap mode on/off in the shader (needs to be done every frame)
*******************************************************************************************************************/
void TerrainShader::SetMinimapMode(bool minimapMode)
{
	//--- Changes every frame and so shader needs to be updated constantly
	SetBool("uniform_terrain_minimapMode", minimapMode);
}