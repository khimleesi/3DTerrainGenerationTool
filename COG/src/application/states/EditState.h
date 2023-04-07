#pragma once

/*******************************************************************************************************************
	PlayState.h, PlayState.cpp
	Created by Kim Kane
	Last updated: 06/01/2018

	Initializes a play state within the game.

*******************************************************************************************************************/
#include <vector>
#include <deque>
#include "memory/Memory.h"
#include "GameState.h"
#include "graphics/shaders/TerrainShader.h"
#include "graphics/shaders/SkyboxShader.h"
#include "graphics/shaders/InterfaceShader.h"
#include "graphics/shaders/TextShader.h"
#include "application/GameComponent.h"
#include "application/Terrain.h"
#include "graphics/Camera.h"
#include "application/Skybox.h"
#include "application/SamplePlayer.h"
#include "graphics/Light.h"

class EditState : public GameState {

private:
	enum ShaderType	{ SHADER_SKYBOX, SHADER_TERRAIN, SHADER_ENTITY, SHADER_INTERFACE, SHADER_TEXT };

public:
	EditState(GameState* previousState);
	virtual ~EditState();

public:
	virtual bool Update()	override;
	virtual bool Render()	override;

private:
	void Initialize();
	void LoadShaders();
	void LoadObjects();
	void LoadComponents();
	void LoadLights();

private:
	void ProcessInput();
	void UpdateObjects();
	void UpdateComponents();

private:
	void RenderWorld();

private:
	Skybox*			m_skybox;
	SamplePlayer*	m_player;
	Camera*			m_mainCamera;
	std::unique_ptr<Terrain> m_terrain;

private:
	int			m_fogType;
	float		m_fogDensity;
	glm::vec4	m_fogColor;
	bool		m_isFogRanged;

	bool		m_isSkyboxTinted;
	glm::vec3	m_skyboxTintColor;
	float		m_tintBegin;
	float		m_tintEnd;

private:
	bool			m_debugMode;
	bool			m_wireFrameMode;
	bool			m_editingMode;

private:
	std::vector<Shader*>			m_shaders;
	std::vector<Light*>				m_lights;
	std::vector<GameComponent*>		m_components;

private:
	static const unsigned int s_maxShaders;
	static const unsigned int s_maxComponents;

private:
	static const float s_defaultCameraZoom;
};