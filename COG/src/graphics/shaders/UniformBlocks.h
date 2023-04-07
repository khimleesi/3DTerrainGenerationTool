#pragma once

/*******************************************************************************************************************
	UniformBlocks.h, UniformBlocks.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A simple header file, that stores a reflection of all of our uniform blocks within our shaders.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include "graphics/Light.h"
#include "graphics/shaders/Shader.h"

namespace shader_constants {

	enum FogType { FOG_LINEAR, FOG_EXP, FOG_EXPSQ };

	const float FOG_DENSITY				= 0.012f;
	const glm::vec4 FOG_COLOR			= glm::vec4(0.15f, 0.17f, 0.20f, 1.0f);//glm::vec4(0.925f, 0.706f, 0.396f, 1.0f);
	const bool IS_FOG_RANGED			= true;

	const bool SKYBOX_TINTED			= true;
	const glm::vec3 SKYBOX_TINT_COLOR	= glm::vec4(0.15f, 0.17f, 0.20f, 1.0f);//glm::vec4(0.925f, 0.706f, 0.396f, 1.0f);
	const float SKYBOX_TINT_BEGIN		= 0.0f;
	const float SKYBOX_TINT_END			= 12.0f;
}

namespace uniform_block {

	//--- Matrices
	struct MatrixData {
		
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 world;
		glm::mat4 intraWorld;
	};

	//--- Texture data
	struct alignas(16) TextureData {
		
		glm::vec2	offset;
		float		rows;
		int			hasFakeLighting;
		int			isMirrored;
	};

	//--- Fog
	struct alignas(16) FogData {
		
		glm::vec4	fogColor;
		int			fogType;
		int			rangeBasedFog;
		float		fogDensity;
	};

	//--- Lights
	struct alignas(16) LightData {
		
		Light		lights[Shader::MAX_LIGHTS];
		glm::vec4	eyePosition;
		int			numLights;
	};

	//--- Material data
	struct MaterialData {

		int		isReflective;
		int		isGlowing;
		int		isNormalMapped;
		float	shininess;
	};
}