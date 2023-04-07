#include <fstream>
#include <pretty_glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "utilities/Log.h"
#include "utilities/vsGLInfoLib.h"
#include "graphics/Camera.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Shader::Shader(const std::string& vertexFileLocation, const std::string& fragmentFileLocation, Camera* camera)	
	:	m_shaderCount(0),
		m_camera(camera),
		m_program(0),
		m_vertexShader(0),
		m_fragmentShader(0)
{
	Load(vertexFileLocation, fragmentFileLocation);
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
Shader::~Shader()
{
	//--- Reset the camera pointer
	if (m_camera) { m_camera = nullptr; }

	//--- Then delete this program
	DestroyProgram();
}


/*******************************************************************************************************************
	Function that initializes the shader
*******************************************************************************************************************/
bool Shader::Load(const std::string& vertexFileLocation, const std::string& fragmentFileLocation)
{
	//--- First, create the program and shader objects and make sure OpenGL doesn't return an error
	if (!CreateProgram()) { return false; }
	if (!CreateShaders()) { return false; }

	std::string src = "Assets\\Shaders\\";

	//--- Try to compile the shader files passed in by the user, return false if there was a problem with compilation
	if (!CompileShader(src + vertexFileLocation))	{ return false; }
	if (!CompileShader(src + fragmentFileLocation))	{ return false; }
	
	//--- If all of the above was successful, attach the shader objects to the program
	AttachShaders();
	
	//--- Check the linking status of our program and return false if there was an issue
	if (!LinkProgram()) { return false; }
	
	//--- Finally, detach the shader objects and delete them
	//--- We do not need them after compilation success (the program now stores the shader data)
	DetachShaders();
	DestroyShaders();

	//--- Output all shader uniform and uniform block information in debug mode
#if COG_DEBUG == 1
	VSGLInfoLib::getUniformsInfo(m_program);
#endif

	return true;
}


/*******************************************************************************************************************
	Function that generates the OpenGL program ID for use
*******************************************************************************************************************/
bool Shader::CreateProgram()
{
	m_program = glCreateProgram();

	if (!m_program) { COG_LOG("[SHADER] Error creating shader program", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else			{ COG_LOG("[SHADER] Program created: ", m_program, LOG_MEMORY); }

	return true;
}


/*******************************************************************************************************************
	Function that generates the OpenGL shader ID's and assigns them to our user-defined shader objects
*******************************************************************************************************************/
bool Shader::CreateShaders()
{
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (!m_vertexShader)	{ COG_LOG("[SHADER] Error creating vertex shader", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else					{ COG_LOG("[SHADER] Created vertex shader: ", m_vertexShader, LOG_MEMORY); }

	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!m_fragmentShader)	{ COG_LOG("[SHADER] Error creating fragment shader", COG_LOG_EMPTY, LOG_ERROR); return false; }
	else					{ COG_LOG("[SHADER] Created fragment shader: ", m_fragmentShader, LOG_MEMORY); }

	return true;
}


/*******************************************************************************************************************
	Function that takes in the shader file, binds it to the shader object and compiles the shader
*******************************************************************************************************************/
bool Shader::CompileShader(const std::string& fileLocation)
{
	GLuint shaderType = 0;

	//--- Distinguish what kind of shader file is passed in and assign the correct shader object
	if (fileLocation.find(".vert") != std::string::npos)				{ shaderType = m_vertexShader; }
	else if (fileLocation.find(".frag") != std::string::npos)			{ shaderType = m_fragmentShader; }
	else { COG_LOG("[SHADER] Invalid shader file extension: ", fileLocation.c_str(), LOG_ERROR); return false; }

	std::string fileData;
	std::ifstream shaderFile;

	//--- Ensure the ifstream object we created above can throw exceptions
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			//--- Create the shader stream that will hold the file data and then open the file
			std::stringstream shaderStream;
			shaderFile.open(fileLocation);

			//--- Read the files buffer contents in to the stream and then close the file
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();

			//--- Convert the stream data in to a string and then store it in to our string
			fileData = shaderStream.str();
		}
		catch (std::ifstream::failure e) { COG_LOG("[SHADER] Problem loading shader file: ", fileLocation.c_str(), LOG_ERROR); return false; }

	COG_LOG("[SHADER] Loaded shader successfully: ", fileLocation.c_str(), LOG_SUCCESS);

	//--- Convert the file data in to a GLchar C-style string, so we can pass it to glShaderSource
	const GLchar* shaderSource = (const GLchar*)fileData.c_str();

	//--- Bind the source code to our shader object
	COG_GLCALL(glShaderSource(shaderType, 1, &shaderSource, NULL));
	
	//--- Compile the shader after we bind it
	COG_GLCALL(glCompileShader(shaderType));

	//--- Check the compile status and flag up any errors that occured
	if (!CompilationSuccess(shaderType))		{ return false; }
	
	
	return true;
}


/*******************************************************************************************************************
	Function that attaches the user defined shader objects to our user defined program object
*******************************************************************************************************************/
void Shader::AttachShaders()
{
	COG_GLCALL(glAttachShader(m_program, m_vertexShader));
	COG_GLCALL(glAttachShader(m_program, m_fragmentShader));

	COG_LOG("[SHADER] Attached vertex shader: ", m_vertexShader, LOG_MEMORY);
	COG_LOG("[SHADER] Attached fragment shader: ", m_fragmentShader, LOG_MEMORY);
}


/*******************************************************************************************************************
	Function that links all our shaders to our program and activates the program
*******************************************************************************************************************/
bool Shader::LinkProgram()
{
	COG_GLCALL(glLinkProgram(m_program));

	//--- Check the linking status and flag up any errors that occured
	if (!CompilationSuccess(m_program)) { return false; }

	return true;
}


/*******************************************************************************************************************
	Function to detach shader object from a program object, opposite from AttachShaders()
*******************************************************************************************************************/
void Shader::DetachShaders()
{
	COG_GLCALL(glDetachShader(m_program, m_vertexShader));
	COG_GLCALL(glDetachShader(m_program, m_fragmentShader));

	COG_LOG("[SHADER] Detached vertex shader: ", m_vertexShader, LOG_MEMORY);
	COG_LOG("[SHADER] Detached fragment shader: ", m_fragmentShader, LOG_MEMORY);
}


/*******************************************************************************************************************
	Function that deletes our shader object, opposite from CreateShaders()
*******************************************************************************************************************/
void Shader::DestroyShaders()
{
	COG_GLCALL(glDeleteShader(m_vertexShader));
	COG_GLCALL(glDeleteShader(m_fragmentShader));

	COG_LOG("[SHADER] Deleted vertex shader: ", m_vertexShader, LOG_MEMORY);
	COG_LOG("[SHADER] Deleted fragment shader: ", m_fragmentShader, LOG_MEMORY);
}


/*******************************************************************************************************************
	Function that deletes our program object, opposite from CreateProgram()
*******************************************************************************************************************/
void Shader::DestroyProgram()
{
	COG_GLCALL(glUseProgram(0));
	COG_GLCALL(glDeleteProgram(m_program));

	COG_LOG("[SHADER] Program destroyed: ", m_program, LOG_MEMORY);
}


/*******************************************************************************************************************
	Error handling function that checks the compile status of our shaders and the linking status of our program
*******************************************************************************************************************/
GLint Shader::CompilationSuccess(GLuint object)
{
	//--- Create a temporary int to store the result of shader compilation
	GLint result = 0;

	//--- Temporary variable that stores the length of the character buffer required to store the information log
	GLsizei length = 0;

	//--- Temporary variable where we will store the information log returned
	GLchar* infoLog = nullptr;

	//--- If the object passed in is a shader object, then we check the compile status, 
	//--- log any errors and output them to the console window
	if (object != m_program) {
		const char* shaderType = (object == m_vertexShader) ? "Vertex Shader" : "Fragment Shader";
		COG_GLCALL(glGetShaderiv(object, GL_COMPILE_STATUS, &result));
		if (result == GL_TRUE) { COG_LOG("[SHADER] Shader compiled successfully: ", shaderType, LOG_SUCCESS); }
		else {
			COG_GLCALL(glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length));
			infoLog = (GLchar*)_malloca(length * sizeof(GLchar));
			COG_GLCALL(glGetShaderInfoLog(object, length, &length, infoLog));
			COG_LOG("[SHADER] Shader failed to compile: ", shaderType, LOG_ERROR);
			COG_LOG("[SHADER] The shader returned the following errors: ", infoLog, LOG_ERROR);
			return result;
		}
	}
	//-- If the object passed in is a program object, then we check the linking status
	//--- store the shader count (we need this later for checking a valid shader exists, before retrieving
	//--- any uniforms), and output any errors to the console window
	else {
		COG_GLCALL(glGetProgramiv(m_program, GL_LINK_STATUS, &result));
		if (result == GL_TRUE) { 
			COG_GLCALL(glGetProgramiv(m_program, GL_ATTACHED_SHADERS, &m_shaderCount));
			COG_LOG("[SHADER] Program linked successfully: ", m_program, LOG_SUCCESS); }
		else {
			COG_GLCALL(glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length));
			infoLog = (GLchar*)_malloca(length * sizeof(GLchar));
			COG_GLCALL(glGetProgramInfoLog(m_program, length, &length, infoLog));
			COG_LOG("[PIPELINE] The program couldn't be linked for the following reason(s): ", infoLog, LOG_ERROR);
			return result;
		}
	}

	return result;
}


/*******************************************************************************************************************
	Function that binds the shader (uses program) for use
*******************************************************************************************************************/
void Shader::Bind() const
{
	COG_GLCALL(glUseProgram(m_program));
}


/*******************************************************************************************************************
	Function that unbinds the shader (stops using the program)
*******************************************************************************************************************/
void Shader::Unbind() const
{
	COG_GLCALL(glUseProgram(0));
}


/*******************************************************************************************************************
	A function that retrieves a uniform variable from a shader file and stores the uniform ID into a map
*******************************************************************************************************************/
bool Shader::GetUniform(const std::string& uniformName)
{
	auto uniform = s_uniforms.find(uniformName);
	if (uniform != s_uniforms.end()) {
		COG_LOG("[SHADER] Uniform already retrieved: ", uniformName.c_str(), LOG_WARN); return false;
	}

	GLint uniformLocation = 0;

	COG_GLCALL(uniformLocation = glGetUniformLocation(m_program, uniformName.c_str()));

	if (uniformLocation == -1 || uniformLocation == GL_INVALID_INDEX) { 
		COG_LOG("[SHADER] Could not find uniform location: ", uniformName.c_str(), LOG_ERROR); return false;
	}
	
	s_uniforms.try_emplace(uniformName, uniformLocation);
	
	return true;
}


/*******************************************************************************************************************
	A function which retrieves a uniform block from the shader and sets up a UBO for this block with the data passed in
*******************************************************************************************************************/
bool Shader::GetUniformBlock(const std::string& uniformBlockName, GLsizeiptr byteSize, GLuint binding, bool dynamic)
{
	if (!ByteSizeMatches(uniformBlockName, byteSize)) { return false; }

	auto uniform = s_uniforms.find(uniformBlockName);
	if (uniform != s_uniforms.end()) {
		COG_LOG("[SHADER] Uniform block already retrieved: ", uniformBlockName.c_str(), LOG_WARN); return false;
	}

	GLint uniformLocation = 0;

	COG_GLCALL(uniformLocation = glGetUniformBlockIndex(m_program, uniformBlockName.c_str()));
	
	if (uniformLocation == -1 || uniformLocation == GL_INVALID_INDEX) {
		COG_LOG("[SHADER] Could not find uniform block location: ", uniformBlockName.c_str(), LOG_ERROR); return false;
	}
	
	COG_GLCALL(glUniformBlockBinding(m_program, uniformLocation, binding));
	
	s_uniforms.try_emplace(uniformBlockName, uniformLocation);

	//--- If UBO doesn't already exist, generate a new UBO for this data
	if (!Resource::Instance()->AddBinding(byteSize, binding, dynamic)) { return false; }

	return true;
}


/*******************************************************************************************************************
	Returns an already bound UBO - note the Resource is a singleton, but I just encapsulated for ease of use
*******************************************************************************************************************/
UniformBuffer* Shader::GetBinding(GLuint binding)
{
	return Resource::Instance()->GetBinding(binding);
}


/*******************************************************************************************************************
	Function that checks if a uniform name exists within our uniforms map
*******************************************************************************************************************/
bool Shader::UniformExists(const std::string& uniformName)
{
	auto uniform = s_uniforms.find(uniformName);

	if (uniform == s_uniforms.end()) {
		COG_LOG("[SHADER] Setting a uniform before retrieving it from shader file: ", uniformName.c_str(), LOG_ERROR);
		return false;
	}

	return true;
}


/*******************************************************************************************************************
	Checks if a uniform block within our shader matches the byte size of the data being passed in
*******************************************************************************************************************/
bool Shader::ByteSizeMatches(const std::string& uniformBlockName, GLsizeiptr byteSize)
{
	GLint shaderBlockSize	= 0;
	GLuint index			= 0;

	COG_GLCALL(index = glGetUniformBlockIndex(m_program, uniformBlockName.c_str()));
	COG_GLCALL(glGetActiveUniformBlockiv(m_program, index, GL_UNIFORM_BLOCK_DATA_SIZE, &shaderBlockSize));

	COG_LOG("[SHADER] Uniform block is: " + std::to_string(shaderBlockSize) + ", data is: ", byteSize, LOG_WARN);

	if (shaderBlockSize != byteSize) {
		COG_LOG("[SHADER] Incorrect alignment for uniform block: ", uniformBlockName.c_str(), LOG_ERROR);
		return false;
	}

	COG_LOG("[SHADER] Alignments match for uniform block: ", uniformBlockName.c_str(), LOG_SUCCESS);

	return true;
}


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Shader::SetMatrix(const std::string& uniformName, const glm::mat4& data)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniformMatrix4fv(s_uniforms[uniformName], 1, GL_FALSE, glm::value_ptr(data))); }
}

void Shader::SetVector2f(const std::string& uniformName, const glm::vec2& value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform2f(s_uniforms[uniformName], value.x, value.y)); }
}

void Shader::SetVector3f(const std::string& uniformName, const glm::vec3& value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform3f(s_uniforms[uniformName], value.x, value.y, value.z)); }
}

void Shader::SetVector4f(const std::string& uniformName, const glm::vec4& value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform4f(s_uniforms[uniformName], value.x, value.y, value.z, value.w)); }
}

void Shader::SetFloat(const std::string& uniformName, const float value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform1f(s_uniforms[uniformName], value)); }
}

void Shader::SetInteger(const std::string& uniformName, const int value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform1i(s_uniforms[uniformName], value)); }
}

void Shader::SetBool(const std::string& uniformName, const bool value)
{
	if (UniformExists(uniformName)) { COG_GLCALL(glUniform1i(s_uniforms[uniformName], value)); }
}

void Shader::SwapCamera(Camera* camera) { m_camera = camera; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
std::map <std::string, GLuint> Shader::s_uniforms;
std::map <Shader::TextureUnit, int> Shader::s_textureUnits = {	{ TEXTURE_DIFFUSE, GL_TEXTURE0 },
																{ TEXTURE_SPECULAR, GL_TEXTURE1 },
																{ TEXTURE_EMISSIVE, GL_TEXTURE2 },
																{ TEXTURE_NORMAL, GL_TEXTURE3 },
																{ TEXTURE_INTERFACE, GL_TEXTURE4 },
																{ TEXTURE_SKYBOX, GL_TEXTURE5 },
																{ TEXTURE_BASE, GL_TEXTURE6 },
																{ TEXTURE_RED, GL_TEXTURE7 },
																{ TEXTURE_GREEN, GL_TEXTURE8 },
																{ TEXTURE_BLUE, GL_TEXTURE9 },
																{ TEXTURE_BLENDMAP, GL_TEXTURE10 },
																{ TEXTURE_BASE_NORMAL, GL_TEXTURE11 },
																{ TEXTURE_RED_NORMAL, GL_TEXTURE12 },
																{ TEXTURE_GREEN_NORMAL, GL_TEXTURE13 },
																{ TEXTURE_BLUE_NORMAL, GL_TEXTURE14 },
																{ TEXTURE_TEXT, GL_TEXTURE15 }
};


/*******************************************************************************************************************
	Static function that returns a texure unit location
*******************************************************************************************************************/
int Shader::GetTextureUnit(TextureUnit unit)
{
	auto textureUnit = s_textureUnits.find(unit);

	if (textureUnit == s_textureUnits.end()) {

		COG_LOG("[SHADER] Texture unit not found: ", unit, LOG_ERROR);
		return -1;
	}

	return textureUnit->second;
}


/*******************************************************************************************************************
	Overridable Virtual functions
*******************************************************************************************************************/
bool Shader::SetLights(const std::vector<Light*>& lights)
{
	return true;
}

void Shader::DebugMode(bool enableDebugSettings)
{

}

/*******************************************************************************************************************
	Operator overloads
*******************************************************************************************************************/
void* Shader::operator new(size_t size)		{ return _aligned_malloc(size, alignof(Shader)); }
void Shader::operator delete(void* memory)	{ _aligned_free(memory); }