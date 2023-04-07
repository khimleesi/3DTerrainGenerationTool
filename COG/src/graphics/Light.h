#pragma once

/*******************************************************************************************************************
	Light.h, Light.cpp
	Created by Kim Kane
	Last updated: 04/03/2018

	A class that creates multiple different lights in our 3D world.

	[Features]
	Supports directional, point and spot lights.
	Has attenuation.
	Ability to switch lights on/off.
	Light margins set up in shader to improve performance.

	[Upcoming]
	Read light data in from an external resource file.

	[Side Notes]
	A light object is a standalone object, primarily to communicate with the shaders.
	It is a replica of a Light struct that is stored within the shaders that have lights (Terrain and Entity).
	Therefore it is not a Game Object of any kind, as it doesn't have any tag/id or a transform.
	It generates its own position in the world by use of a position and direction vector.
	You can attach the light to any object in the scene by setting its position/direction relative to the objects
	transform.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include <string>

/*******************************************************************************************************************
	Helper structs. These are purely to help make our code more readable, as lights' take a lot of parameters
*******************************************************************************************************************/

struct Position {
	float x, y, z;
	Position(float x, float y, float z) : x(x), y(y), z(z) {}
	Position(const glm::vec3& position) : x(position.x), y(position.y), z(position.z) {}
};

struct Direction {
	float x, y, z;
	Direction(float x, float y, float z) : x(x), y(y), z(z) {}
	Direction(const glm::vec3& direction) : x(direction.x), y(direction.y), z(direction.z) {}
};

struct Attenuation {
	float c, l, q;
	Attenuation(float c, float l, float q) : c(c), l(l), q(q) {}
};

struct Ambient {
	float r, g, b;
	Ambient(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct Diffuse {
	float r, g, b;
	Diffuse(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct Specular {
	float r, g, b;
	Specular(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct Angle {
	float inner, outer;
	Angle(float inner, float outer) : inner(inner), outer(outer) {}
};

class Light {

public:
	enum LightType : int { LIGHT_DIRECTION, LIGHT_POINT, LIGHT_SPOT };

public:
	Light(const Direction& direction,
		  const Ambient& ambient, const Diffuse& diffuse, const Specular& specular);
	
	Light(const Position& position,
		  const Ambient& ambient, const Diffuse& diffuse, const Specular& specular,
		  const Attenuation& attenuation, float margin);

	Light(const Position& position, const Direction& direction,
		  const Ambient& ambient, const Diffuse& diffuse, const Specular& specular,
		  const Attenuation& attenuation, const Angle& angle, float margin);

public:
	Light();
	~Light();

public:
	static Light* Create(const std::string& tag);

public:
	const glm::vec4& GetPosition() const;
	const glm::vec4& GetDirection() const;
	
public:
	const glm::vec4& GetAmbient() const;
	const glm::vec4& GetDiffuse() const;
	const glm::vec4& GetSpecular() const;

public:
	bool IsOfType(LightType type) const;

public:
	float GetConstant() const;
	float GetLinear() const;
	float GetQuadratic() const;

public:
	float GetCutOff() const;
	float GetOuterCutOff() const;
	float GetMargin() const;

public:
	int IsEnabled() const;

public:
	void SetPosition(const glm::vec3& position);
	void SetDirection(const glm::vec3& direction);

public:
	void SetAmbient(const glm::vec3& ambient);
	void SetDiffuse(const glm::vec3& diffuse);
	void SetSpecular(const glm::vec3& specular);

public:
	void SetConstant(float constant);
	void SetLinear(float linear);
	void SetQuadratic(float quadratic);

public:
	void SetCutOff(float cutoff);
	void SetOuterCutOff(float outerCutoff);
	void SetMargin(float margin);

public:
	void SetEnabled(bool isEnabled);

public:
	bool operator==(const Light& other) const;

private:
	//--- Remember the base 16 rules!
	 glm::vec4  m_position;
	 glm::vec4  m_direction;
	 glm::vec4	m_ambient;
	 glm::vec4	m_diffuse;
	 glm::vec4	m_specular;

private:
	 LightType	m_type;
	 float		m_constant;
	 float		m_linear;
	 float		m_quadratic;

private:
	 float	m_cutOff;
	 float	m_outerCutOff;
	 int	m_isEnabled;
	 float	m_margin;
};