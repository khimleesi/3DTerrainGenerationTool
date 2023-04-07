#include <vector>
#include <pretty_glm/gtc/matrix_transform.hpp>

#include "ScreenManager.h"
#include "managers/InputManager.h"
#include "utilities/Log.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
ScreenManager::ScreenManager()	:	m_title("SDL OPEN GL Window"),
									m_width(screen_constants::WIDTH),
									m_height(screen_constants::HEIGHT),
									m_version(screen_constants::OPENGL_VERSION),
									m_subVersion(screen_constants::OPENGL_SUBVERSION),
									m_fullScreen(false),
									m_coreMode(false),
									m_vSync(false),
									m_fieldOfView(screen_constants::MAX_FIELD_OF_VIEW),
									m_anisotropy(0.0f),
									m_isAnisotropySupported(false),
									m_window(nullptr),
									m_context(nullptr),
									m_projection(1.0f),
									m_perspective(1.0f),
									m_orthographic(1.0f)
{
	COG_LOG("[SCREEN MANAGER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
void ScreenManager::ShutDown()
{
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	COG_LOG("[SCREEN MANAGER SHUT DOWN]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Function that initializes all necessary startup procedures
*******************************************************************************************************************/
bool ScreenManager::Initialize(	const std::string& title,
								GLuint width,
								GLuint height,
								GLuint version,
								GLuint subVersion,
								bool fullScreen,
								bool coreMode,
								bool vSync)
{
	m_title			= title;
	m_width			= width;
	m_height		= height;
	m_version		= version;
	m_subVersion	= subVersion;
	m_fullScreen	= fullScreen;
	m_coreMode		= coreMode;
	m_vSync			= vSync;
	
	//--- Check SDL initialization ran OK before creating an OpenGL context
	if (InitializeSDL())		{ CreateOpenGLContext(); }
	else						{ return false; }

	//--- Check GLEW ran OK before displaying the GPU extensions
	if (DisplayGPUProfile())	{ DisplayGPUExtensions(); }
	else						{ return false; }

	//--- If everything was initialized correctly, initialize OpenGL default graphics settings
	InitializeGraphics();

	return true;
}


/*******************************************************************************************************************
	Function that enables/disables blending (seperate bool so they don't pass in something silly)
*******************************************************************************************************************/
void ScreenManager::EnableBlending(bool enableBlending)
{
	if (enableBlending) { COG_GLCALL(glEnable(GL_BLEND)); COG_GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); }
	else				{ COG_GLCALL(glDisable(GL_BLEND)); }
}


/*******************************************************************************************************************
	Function that enables/disables depth (seperate bool so they don't pass in something silly)
*******************************************************************************************************************/
void ScreenManager::EnableDepth(bool enableDepth)
{
	if (enableDepth)	{ COG_GLCALL(glEnable(GL_DEPTH_TEST));}
	else				{ COG_GLCALL(glDisable(GL_DEPTH_TEST)); }
}


/*******************************************************************************************************************
	Function that enables/disables depth writing (seperate bool so they don't pass in something silly)
*******************************************************************************************************************/
void ScreenManager::EnableDepthWriting(bool enableDepthWriting)
{
	if (enableDepthWriting) { COG_GLCALL(glDepthMask(GL_TRUE)); }
	else					{ COG_GLCALL(glDepthMask(GL_FALSE)); }
}


/*******************************************************************************************************************
	Function that enables/disables culling of the back face (seperate bool so they don't pass in something silly)
*******************************************************************************************************************/
void ScreenManager::CullBackFace(bool cull)
{
	if (cull)	{ COG_GLCALL(glEnable(GL_CULL_FACE)); COG_GLCALL(glCullFace(GL_BACK)); }
	else		{ COG_GLCALL(glDisable(GL_CULL_FACE)); }
}


/*******************************************************************************************************************
	Function that enables/disables wireframe mode (mainly for debugging purposes only)
*******************************************************************************************************************/
void ScreenManager::WireframeMode(bool wireframe)
{
	if (wireframe)	{ COG_GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); }
	else			{ COG_GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); }
}


/*******************************************************************************************************************
	Function that initializes all default OpenGL graphics settings
*******************************************************************************************************************/
void ScreenManager::InitializeGraphics()
{
	SetDefaultViewport();
	
	//--- Explicity set the winding order (default is counter-clockwise)
	glFrontFace(GL_CCW);

	SetupAnisotropicFiltering();

	//--- Exponential correction; increase gamma so colours appear brighter
	//--- Not currently enabled as I have not setup gamma properly in shaders yet
	//glEnable(GL_FRAMEBUFFER_SRGB);

	InitializePerspective();
	InitializeOrthographic();
}


/*******************************************************************************************************************
	Initializes a perspective (3D) view using a matrix
*******************************************************************************************************************/
void ScreenManager::InitializePerspective()
{
	using namespace screen_constants;

	m_perspective			= glm::perspective(m_fieldOfView, GetAspectRatio(), NEAR_VIEW, FAR_VIEW);
	m_defaultPerspective	= glm::perspective(MAX_FIELD_OF_VIEW, GetAspectRatio(), NEAR_VIEW, FAR_VIEW);
}


/*******************************************************************************************************************
	Initializes an orthographic (2D) view using a matrix
*******************************************************************************************************************/
void ScreenManager::InitializeOrthographic()
{
	using namespace screen_constants;

	m_orthographic = glm::ortho(0.0f, (float)m_width, 0.0f, (float)m_height, NEAR_VIEW, FAR_VIEW);
}


/*******************************************************************************************************************
	Toggle that switches between 2D and 3D views, default perspective matrix needed for minimaps (disables cam zoom)
*******************************************************************************************************************/
void ScreenManager::PerspectiveView(bool perspective, bool useDefault)
{
	(perspective)	? (useDefault) ? m_projection = m_defaultPerspective	: m_projection = m_perspective
																			: m_projection = m_orthographic;
}


/*******************************************************************************************************************
	Function that clears the screen color and depth buffer every frame
*******************************************************************************************************************/
void ScreenManager::BeginScene(float r, float g, float b)
{	
	COG_GLCALL(glClearColor(r, g, b, 1.0f));
	COG_GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}


/*******************************************************************************************************************
	Function that swaps the buffers (necessary for drawing to the screen)
*******************************************************************************************************************/
void ScreenManager::EndScene()
{
	SDL_GL_SwapWindow(m_window);
}


/*******************************************************************************************************************
	Function that initializes the SDL Window
*******************************************************************************************************************/
bool ScreenManager::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)	{ COG_LOG("[SCREEN] SDL failed to initialize", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else										{ COG_LOG("[SCREEN] SDL initialized successfully!", COG_LOG_EMPTY, LOG_SUCCESS); }

	//--- Set our OpenGL attributes before creating window
	SetOpenGLAttributes();

	Uint32 screenFlag = (m_fullScreen) ? SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN : SDL_WINDOW_OPENGL;

	m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, screenFlag);

	if (!m_window)	{ COG_LOG("[SCREEN] Game window could not be created", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else			{ COG_LOG("[SCREEN] Game window created successfully!", COG_LOG_EMPTY, LOG_SUCCESS); }

	return true;
}


/*******************************************************************************************************************
	Function that sets all our OpenGL attributes, the context and the context version
*******************************************************************************************************************/
void ScreenManager::SetOpenGLAttributes() {
	
	//--- Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	
	//---- Enable multisampling
	glEnable(GL_MULTISAMPLE);

	//--- Set double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//--- Check if OpenGL profile is core or compatibility
	if (m_coreMode)	{ SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
					  COG_LOG("[SCREEN] OpenGL CORE mode enabled", COG_LOG_EMPTY, LOG_MESSAGE); }
	else			{ SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); 
					  COG_LOG("[SCREEN] OpenGL COMPATIBILITY mode enabled", COG_LOG_EMPTY, LOG_MESSAGE); }

	//--- Set OpenGL context version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_version); COG_LOG("[SCREEN] OpenGL version in use: ", m_version, LOG_MESSAGE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_subVersion); COG_LOG("[SCREEN] OpenGL sub version in use: ", m_subVersion, LOG_MESSAGE);
}


/*******************************************************************************************************************
	Function that creates the OpenGL context
*******************************************************************************************************************/
bool ScreenManager::CreateOpenGLContext()
{
	//--- Create OpenGL context
	m_context = SDL_GL_CreateContext(m_window);

	if (!m_context) { COG_LOG("[SCREEN] OpenGL context could not be created", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else			{ COG_LOG("[SCREEN] OpenGL context created successfully!", COG_LOG_EMPTY, LOG_SUCCESS); }

	if (m_vSync) {
		if (SDL_GL_SetSwapInterval(1) == -1) { 
			COG_LOG("[SCREEN] VSync not supported", COG_LOG_EMPTY, LOG_MESSAGE);
		} }
	else {
		if (SDL_GL_SetSwapInterval(0) == -1) { 
			COG_LOG("[SCREEN] VSync not supported", COG_LOG_EMPTY, LOG_MESSAGE);
		} }

	if (SDL_GL_GetSwapInterval() != (int)m_vSync) { 
		COG_LOG("[SCREEN] VSync settings determined by your graphics card", COG_LOG_EMPTY, LOG_MESSAGE);
	}

	COG_LOG("[SCREEN] VSync: ", (int)m_vSync, LOG_MESSAGE);
	
	return true;
}


/*******************************************************************************************************************
	Function that initializes GLEW and displays GPU information to the console window
*******************************************************************************************************************/
bool ScreenManager::DisplayGPUProfile()
{
	glewExperimental = GL_TRUE;

	//--- Initialize GLEW and display GPU information
	if (glewInit() != GLEW_OK) { COG_LOG("[SCREEN] Could not initialize GLEW", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else {
		COG_LOG("[SCREEN] GPU Manufacturer: ", glGetString(GL_VENDOR), LOG_MESSAGE);
		COG_LOG("[SCREEN] GPU Make and Model: ", glGetString(GL_RENDERER), LOG_MESSAGE);
		COG_LOG("[SCREEN] Current OpenGL version and mode: ", glGetString(GL_VERSION), LOG_MESSAGE);
		COG_LOG("[SCREEN] GLSL Language supported: ", glGetString(GL_SHADING_LANGUAGE_VERSION), LOG_MESSAGE);
	}

	return true;
}


/*******************************************************************************************************************
	Sets the default viewport of the game window
*******************************************************************************************************************/
void ScreenManager::SetDefaultViewport()
{
	glViewport(0, 0, m_width, m_height);
}


/*******************************************************************************************************************
	Adjusts the viewport by a width and height passed to function
*******************************************************************************************************************/
void ScreenManager::SetCustomViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}


/*******************************************************************************************************************
	Retrieves all the supported extensions from the users GPU and displays them to the console window
*******************************************************************************************************************/
void ScreenManager::DisplayGPUExtensions()
{
	GLint numExtensions = 0;

	std::vector <const GLubyte*> extensions;

	COG_GLCALL(glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions));

	extensions.reserve(numExtensions);

	for (GLint x = 0; x < numExtensions; x++) { extensions.emplace_back(glGetStringi(GL_EXTENSIONS, x)); }
	
	COG_LOG("[EXTENSIONS] Supported Extensions on GPU:", COG_LOG_EMPTY, LOG_MESSAGE);

	for (GLuint x = 0; x < extensions.size(); x++) { COG_LOG("[EXT] ", extensions[x], LOG_MESSAGE); }
}


/*******************************************************************************************************************
	A function that checks if anisotropic filtering is enabled and stores the max value available
*******************************************************************************************************************/
void ScreenManager::SetupAnisotropicFiltering()
{
	//--- If anisotropic filtering is supported then use it
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic") || GLEW_EXT_texture_filter_anisotropic)
	{
		GLfloat maxAnisotropy		= 0.0f;
		GLfloat desiredAnisotropy	= 4.0f;
		
		COG_GLCALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy));
		
		m_anisotropy			= std::min(desiredAnisotropy, maxAnisotropy);
		
		m_isAnisotropySupported = true;
		
		COG_LOG("[SCREEN] Anisotropic filtering is supported. Hello fancy graphics!", COG_LOG_EMPTY, LOG_SUCCESS);
	}
	else { 
		COG_LOG("[SCREEN] Anisotropic filtering not supported on your graphics card", COG_LOG_EMPTY, LOG_WARN); 
		m_isAnisotropySupported = false;
	}
}


/*******************************************************************************************************************
	Accessor Methods
*******************************************************************************************************************/
SDL_Window* ScreenManager::GetWindow() const				{ return m_window; }
float ScreenManager::GetAspectRatio() const					{ return (float)m_width / (float)m_height; }
float ScreenManager::GetWidth() const						{ return (float)m_width; }
float ScreenManager::GetHeight() const						{ return (float)m_height; }
float ScreenManager::GetFieldOfView() const					{ return m_fieldOfView; }
const glm::mat4& ScreenManager::GetProjectionMatrix() const	{ return m_projection; }
float ScreenManager::GetAnisotropy() const					{ return m_anisotropy; }
bool ScreenManager::IsAnisotropySupported() const			{ return m_isAnisotropySupported; }


/*******************************************************************************************************************
	Modifier Methods
*******************************************************************************************************************/
void ScreenManager::SetFieldOfView(float fieldOfView)
{
	using namespace screen_constants;

	m_perspective = glm::perspective(m_fieldOfView += fieldOfView, GetAspectRatio(), NEAR_VIEW, FAR_VIEW);
}