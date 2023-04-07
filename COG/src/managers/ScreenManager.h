#pragma once

/*******************************************************************************************************************
	ScreenManager.h, ScreenManager.cpp
	Created by Kim Kane
	Last updated: 05/01/2018

	A singleton class that handles all screen and graphical setup.

	[Features]
	Supports SDL and OpenGL.
	Supports fullscreen and windowed modes.
	Supports anisotropy filtering.
	Supports perspective and orthographic screen projections (plus default perspective for when using minimaps)
	Simple functions to toggle different graphical settings on/off.

	[Upcoming]
	Ability to toggle fullscreen/windowed mode on/off within the game.
	Multiple resolution support (reading the resolution from the users PC, as opposed to hard-coding it)

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_sdl/sdl.h>
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <string>

#include "utilities/Singleton.h"

namespace screen_constants {

	enum WindowDefaults : GLuint { WIDTH = 1024, HEIGHT = 768, OPENGL_VERSION = 4, OPENGL_SUBVERSION = 0 };

	const float MAX_FIELD_OF_VIEW	= glm::radians(45.0f);
	const float MIN_FIELD_OF_VIEW	= glm::radians(5.0f);
	const float NEAR_VIEW			= 0.1f;
	const float FAR_VIEW			= 1000.0f;
}

class ScreenManager {

public:
	friend class Singleton<ScreenManager>;
	
public:
	bool Initialize(const std::string& title,
					GLuint width,
					GLuint height,
					GLuint version,
					GLuint subVersion,
					bool fullScreen = false,
					bool core = false,
					bool vSync = false);
public:
	void ShutDown();

public:
	void BeginScene(float r, float g, float b);
	void EndScene();

public:
	SDL_Window*			GetWindow() const;
	float				GetAspectRatio() const;
	float				GetWidth() const;
	float				GetHeight() const;
	float				GetFieldOfView() const;
	const glm::mat4&	GetProjectionMatrix() const;
	float				GetAnisotropy() const;
	bool				IsAnisotropySupported() const;

public:
	void SetFieldOfView(float fieldOfView);
	void SetDefaultViewport();
	void SetCustomViewport(int width, int height);

public:
	void EnableBlending(bool enableBlending);
	void EnableDepth(bool enableDepth);
	void EnableDepthWriting(bool enableDepthWriting);
	void CullBackFace(bool cull);
	void PerspectiveView(bool perspective, bool useDefault = false);
	void WireframeMode(bool wireframe);

private:
	ScreenManager();
	ScreenManager(const ScreenManager&)				= delete;
	ScreenManager& operator=(const ScreenManager&)	= delete;

private:
	bool InitializeSDL();
	void SetOpenGLAttributes();
	bool CreateOpenGLContext();
	bool DisplayGPUProfile();
	void DisplayGPUExtensions();
	void InitializeGraphics();

private:
	void InitializeOrthographic();
	void InitializePerspective();
	void SetupAnisotropicFiltering();

private:
	std::string		m_title;
	GLuint			m_width, m_height;
	GLuint			m_version, m_subVersion;

	bool			m_fullScreen;
	bool			m_coreMode;
	bool			m_vSync;

	float			m_fieldOfView;
	float			m_anisotropy;
	bool			m_isAnisotropySupported;

	SDL_Window*		m_window;
	SDL_GLContext	m_context;

	glm::mat4		m_projection;
	glm::mat4		m_perspective;
	glm::mat4		m_orthographic;

	glm::mat4		m_defaultPerspective;
};

typedef Singleton<ScreenManager> Screen;