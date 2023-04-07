#pragma once

/*******************************************************************************************************************


*******************************************************************************************************************/
#include "GameState.h"
#include "graphics/shaders/InterfaceShader.h"
#include "application/Background.h"
#include "application/Button.h"

class StoryState : public GameState {

public:
	StoryState(GameState* previousState);
	virtual ~StoryState();

public:
	virtual bool Update()	override;
	virtual bool Render()	override;

private:
	bool Initialize();
	void LoadShaders();
	void LoadInterface();
	void ProcessInput();

private:
	Shader*			m_shader;
	Background*		m_background;
	Button*			m_return;
}; 
