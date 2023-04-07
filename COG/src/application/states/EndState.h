#pragma once

/*******************************************************************************************************************


*******************************************************************************************************************/
#include "GameState.h"
#include "graphics/shaders/InterfaceShader.h"
#include "application/Background.h"
#include "application/Button.h"

class EndState : public GameState {

public:
	EndState(GameState* previousState);
	virtual ~EndState();

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
	Button*			m_menu;
	Button*			m_play;
};
