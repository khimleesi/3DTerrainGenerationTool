#pragma once

/*******************************************************************************************************************
	Shader.h, Shader.cpp
	Created by Kim Kane
	Last updated: 07/04/2018

	A base class for all shaders in our program.

	[Features]
	Supports vertex and fragment shader loading, with complete error checking features and debug message outputs.
	Supports individual uniform variables as well as uniform blocks.
	Bindings are created once in cache memory (providing you use the resource manager) and can be re-used.
	Has a static uniform map to keep track of all uniforms loaded; good for debugging.

	[Upcoming]
	Shader caching - program ID's will be kept in memory for re-use (needs testing)
	Shared UBO blocks amongst shaders (needs testing)
	Deferred rendering support and geometry shader support.

	[Side Notes]
	When using uniform blocks in shaders we must make sure that the data we pass in 
	is properly aligned to a base of 16 bytes (dividable by 16), more information can 
	be found here: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
	and here: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
	A nice solution to this is to align everything properly on the CPU before sending the data to the shader.
	We can also send the data in as a complete struct with an offset of 0. This tells the shader
	to start from the beginning of the uniform block and work through each variable, assigning values accordingly.
	The only thing we must do is keep variables in order, in every shader and every struct.
	When we declare data has base 16 (alignas(16)), the compiler automatically
	generates padding (at the end) of our struct to make it 16 byte aligned.
	Vec3's are generally not used for uniform blocks and should be avoided.
	Reference: https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o/38172697#38172697
	We can also make use of the handy vsGLInfoLib library by LightHouse 3D, to check our
	offsets and sizes match up properly in our shaders (Call function: VSGLInfoLib::getUniformsInfo(programID)).
	
	A properly aligned struct of data should look something like this:
	
	struct alignas(16) SomeData {
		
		glm::vec2 someVec;	- Offset 0, base alignment of 8 in GLSL, 8 bytes of data				- Total bytes : 8
		float someFloat;	- +8 bytes = Offset 8, base alignment of 4 in GLSL, 4 bytes of data		- Total bytes : 12
		int someInt1;		- +4 bytes = Offset 12, base alignment of 4 in GLSL, 4 bytes of data	- Total bytes : 16
		int someInt2;		- +4 bytes = Offset 16, base alignment of 4 in GLSL, 4 bytes of data	- Total bytes : 20
		
		We have 20 bytes total, so the compiler will add a padding of 12 bytes (32 bytes in total)
		to make our struct 16 byte aligned (16 + 16 = 32)

		In memory it will look something like this:
		float somePadding1;
		float somePadding2;
		float somePadding3;
	};

	*You should be wary of using boolean variables within GLSL - they don't always work correctly (tried and tested),
	and they have a base alignment of 1 byte. I find it safer, easier and more accurate
	to use integer's instead, but that's just my opinion!

*******************************************************************************************************************/
#include <pretty_opengl/glew.h>
#include <pretty_glm/glm.hpp>
#include <string>
#include <map>
#include <vector>
#include "managers/ResourceManager.h"

class Camera; class Transform; class Texture; class Material; class Light;

class alignas(16) Shader {

public:
	static const unsigned int MAX_LIGHTS = 15;

public:
	enum TextureUnit {
		TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_EMISSIVE, TEXTURE_NORMAL, TEXTURE_INTERFACE, TEXTURE_SKYBOX,
		TEXTURE_BASE, TEXTURE_RED, TEXTURE_GREEN, TEXTURE_BLUE, TEXTURE_BLENDMAP, TEXTURE_BASE_NORMAL,
		TEXTURE_RED_NORMAL, TEXTURE_GREEN_NORMAL, TEXTURE_BLUE_NORMAL, TEXTURE_TEXT
	};

protected:
	enum BindingSlot {
		BIND_ENTITY_MATRIX_DATA, BIND_ENTITY_TEXTURE_DATA, BIND_ENTITY_FOG_DATA, BIND_ENTITY_LIGHT_DATA,
		BIND_ENTITY_MATERIAL_DATA, BIND_TERRAIN_MATRIX_DATA, BIND_TERRAIN_TEXTURE_DATA,
		BIND_TERRAIN_FOG_DATA, BIND_TERRAIN_LIGHT_DATA
	};

public:
	Shader(const std::string& vertex, const std::string& fragment, Camera* camera = nullptr);
	virtual ~Shader();

public:
	void* operator new(size_t size);
	void operator delete(void* memory);

public:
	void Bind() const;
	void Unbind() const;

public:
	virtual bool SetLights(const std::vector<Light*>& lights);
	virtual void DebugMode(bool enableDebugSettings);
	virtual void SetFogData(int type, bool rangeBased, float density, const glm::vec4& color) {}
	virtual void SetSkyboxData(bool isTintEnabled, float tintBegin, float tintEnd, const glm::vec3& tintColor) {}

public:
	void SwapCamera(Camera* camera);

public:
	static int GetTextureUnit(TextureUnit unit);

protected:
	virtual void GetAllUniforms()			= 0;
	virtual void SetPermanentAttributes()	= 0;

protected:
	bool GetUniform(const std::string& uniformName);
	bool GetUniformBlock(const std::string& uniformBlockName, GLsizeiptr byteSize, GLuint binding, bool dynamic = false);
	UniformBuffer* GetBinding(GLuint binding);

protected:
	void SetMatrix(const std::string& uniformName, const glm::mat4& data);
	void SetVector2f(const std::string& uniformName, const glm::vec2& value);
	void SetVector3f(const std::string& uniformName, const glm::vec3& value);
	void SetVector4f(const std::string& uniformName, const glm::vec4& value);
	void SetFloat(const std::string& uniformName, const float value);
	void SetInteger(const std::string& uniformName, const int value);
	void SetBool(const std::string& uniformName, const bool value);

private:
	Shader(Shader const&)				= delete;
	Shader& operator=(Shader const&)	= delete;

private:
	bool Load(const std::string& vertexFileLocation, const std::string& fragmentFileLocation);
	bool CreateShaders();
	bool CompileShader(const std::string& fileLocation);
	void AttachShaders();
	void DetachShaders();
	void DestroyShaders();

private:
	bool CreateProgram();
	bool LinkProgram();
	void DestroyProgram();

private:
	GLint CompilationSuccess(GLuint object);
	bool UniformExists(const std::string& uniformName);
	bool ByteSizeMatches(const std::string& uniformBlockName, GLsizeiptr byteSize);

protected:
	GLint	m_shaderCount;
	Camera* m_camera;

private:
	GLuint	m_program;
	GLuint	m_vertexShader;
	GLuint	m_fragmentShader;

private:
	static std::map<std::string, GLuint>	s_uniforms;
	static std::map<TextureUnit, int>		s_textureUnits;
};