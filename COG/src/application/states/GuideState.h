#pragma once

/*******************************************************************************************************************


*******************************************************************************************************************/
#include "GameState.h"
#include "graphics/shaders/InterfaceShader.h"
#include "application/Background.h"
#include "application/Button.h"

class GuideState : public GameState {

public:
	GuideState(GameState* previousState);
	virtual ~GuideState();

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
