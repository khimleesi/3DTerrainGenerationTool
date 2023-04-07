#include "Light.h"
#include "managers/ReaderManager.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	[Directional Light] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Light::Light(const Direction& direction,
			 const Ambient& ambient, const Diffuse& diffuse, const Specular& specular)

	:	m_position(0.0f),
		m_direction(direction.x, direction.y, direction.z, 1.0f),
		m_ambient(ambient.r, ambient.g, ambient.b, 1.0f),
		m_diffuse(diffuse.r, diffuse.g, diffuse.b, 1.0f),
		m_specular(specular.r, specular.g, specular.b, 1.0f),
		m_type(LIGHT_DIRECTION),
		m_constant(0.0f),
		m_linear(0.0f),
		m_quadratic(0.0f),
		m_cutOff(0.0f),
		m_outerCutOff(0.0f),
		m_isEnabled(1),
		m_margin(0.0f)
{

}


/*******************************************************************************************************************
	[Point Light] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Light::Light(const Position& position,
			 const Ambient& ambient, const Diffuse& diffuse, const Specular& specular,
			 const Attenuation& attenuation, float margin)

	:	m_position(position.x, position.y, position.z, 1.0f),
		m_direction(0.0f),
		m_ambient(ambient.r, ambient.g, ambient.b, 1.0f),
		m_diffuse(diffuse.r, diffuse.g, diffuse.b, 1.0f),
		m_specular(specular.r, specular.g, specular.b, 1.0f),
		m_type(LIGHT_POINT),
		m_constant(attenuation.c),
		m_linear(attenuation.l),
		m_quadratic(attenuation.q),
		m_cutOff(0.0f),
		m_outerCutOff(0.0f),
		m_isEnabled(1),
		m_margin(margin)
{

}


/*******************************************************************************************************************
	[Spot Light] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Light::Light(const Position& position, const Direction& direction,
			 const Ambient& ambient, const Diffuse& diffuse, const Specular& specular,
			 const Attenuation& attenuation, const Angle& angle, float margin)

	:	m_position(position.x, position.y, position.z, 1.0f),
		m_direction(direction.x, direction.y, direction.z, 1.0f),
		m_ambient(ambient.r, ambient.g, ambient.b, 1.0f),
		m_diffuse(diffuse.r, diffuse.g, diffuse.b, 1.0f),
		m_specular(specular.r, specular.g, specular.b, 1.0f),
		m_type(LIGHT_SPOT),
		m_constant(attenuation.c),
		m_linear(attenuation.l),
		m_quadratic(attenuation.q),
		m_cutOff(glm::cos(glm::radians(angle.inner))),
		m_outerCutOff(glm::cos(glm::radians(angle.outer))),
		m_isEnabled(1),
		m_margin(margin)
{

}


/*******************************************************************************************************************
	[Default] Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Light::Light()
	:	m_position(0.0f),
		m_direction(0.0f),
		m_ambient(0.0f),
		m_diffuse(0.0f),
		m_specular(0.0f),
		m_type(LIGHT_DIRECTION),
		m_constant(0.0f),
		m_linear(0.0f),
		m_quadratic(0.0f),
		m_cutOff(0.0f),
		m_outerCutOff(0.0f),
		m_isEnabled(0),
		m_margin(0.0f)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Light::~Light()
{

}


/*******************************************************************************************************************
	Static function which creates a light instance from data read in from a file
*******************************************************************************************************************/
Light* Light::Create(const std::string& tag)
{
	Reader::Instance()->OpenForReading("Assets\\Files\\lights.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);
	
	Light* light = nullptr;

	LightType type = (LightType)StringToInteger(data[tag]["type"]);
	
	switch (type) {

		case LIGHT_DIRECTION: {
			light = new Light(
				Direction(StringToFloat(data[tag]["direction.x"]), StringToFloat(data[tag]["direction.y"]), StringToFloat(data[tag]["direction.z"])),
				Ambient(StringToFloat(data[tag]["ambient.r"]), StringToFloat(data[tag]["ambient.g"]), StringToFloat(data[tag]["ambient.b"])),
				Diffuse(StringToFloat(data[tag]["diffuse.r"]), StringToFloat(data[tag]["diffuse.g"]), StringToFloat(data[tag]["diffuse.b"])),
				Specular(StringToFloat(data[tag]["specular.r"]), StringToFloat(data[tag]["specular.g"]), StringToFloat(data[tag]["specular.b"]))); 
			break;
		}

		case LIGHT_POINT: {
			light = new Light(
				Position(StringToFloat(data[tag]["position.x"]), StringToFloat(data[tag]["position.y"]), StringToFloat(data[tag]["position.z"])),
				Ambient(StringToFloat(data[tag]["ambient.r"]), StringToFloat(data[tag]["ambient.g"]), StringToFloat(data[tag]["ambient.b"])),
				Diffuse(StringToFloat(data[tag]["diffuse.r"]), StringToFloat(data[tag]["diffuse.g"]), StringToFloat(data[tag]["diffuse.b"])),
				Specular(StringToFloat(data[tag]["specular.r"]), StringToFloat(data[tag]["specular.g"]), StringToFloat(data[tag]["specular.b"])),
				Attenuation(StringToFloat(data[tag]["attenuation.c"]), StringToFloat(data[tag]["attenuation.l"]), StringToFloat(data[tag]["attenuation.q"])),
				StringToFloat(data[tag]["margin"]));
			break;
		}

		case LIGHT_SPOT: {
			light = new Light(
				Position(StringToFloat(data[tag]["position.x"]), StringToFloat(data[tag]["position.y"]), StringToFloat(data[tag]["position.z"])),
				Direction(StringToFloat(data[tag]["direction.x"]), StringToFloat(data[tag]["direction.y"]), StringToFloat(data[tag]["direction.z"])),
				Ambient(StringToFloat(data[tag]["ambient.r"]), StringToFloat(data[tag]["ambient.g"]), StringToFloat(data[tag]["ambient.b"])),
				Diffuse(StringToFloat(data[tag]["diffuse.r"]), StringToFloat(data[tag]["diffuse.g"]), StringToFloat(data[tag]["diffuse.b"])),
				Specular(StringToFloat(data[tag]["specular.r"]), StringToFloat(data[tag]["specular.g"]), StringToFloat(data[tag]["specular.b"])),
				Attenuation(StringToFloat(data[tag]["attenuation.c"]), StringToFloat(data[tag]["attenuation.l"]), StringToFloat(data[tag]["attenuation.q"])),
				Angle(StringToFloat(data[tag]["angle.inner"]), StringToFloat(data[tag]["angle.outer"])),
				StringToFloat(data[tag]["margin"]));
			break;
		}
	}

	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return light;
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec4& Light::GetPosition() const		{ return m_position; }
const glm::vec4& Light::GetDirection() const	{ return m_direction; }

const glm::vec4& Light::GetAmbient() const		{ return m_ambient; }
const glm::vec4& Light::GetDiffuse() const		{ return m_diffuse; }
const glm::vec4& Light::GetSpecular() const		{ return m_specular; }

bool Light::IsOfType(LightType type) const		{ return m_type == type; }

float Light::GetConstant() const				{ return m_constant; }
float Light::GetLinear() const					{ return m_linear; }
float Light::GetQuadratic() const				{ return m_quadratic; }

float Light::GetCutOff() const					{ return m_cutOff; }
float Light::GetOuterCutOff() const				{ return m_outerCutOff; }
float Light::GetMargin() const					{ return m_margin;}
int Light::IsEnabled() const					{ return m_isEnabled; }


/*******************************************************************************************************************
	Modifier methods
*******************************************************************************************************************/
void Light::SetPosition(const glm::vec3& position)		{ m_position = glm::vec4(position, 1.0f); }
void Light::SetDirection(const glm::vec3& direction)	{ m_direction = glm::vec4(direction, 1.0f); }

void Light::SetAmbient(const glm::vec3& ambient)		{ m_ambient = glm::vec4(ambient, 1.0f); }
void Light::SetDiffuse(const glm::vec3& diffuse)		{ m_diffuse = glm::vec4(diffuse, 1.0f); }
void Light::SetSpecular(const glm::vec3& specular)		{ m_specular = glm::vec4(specular, 1.0f); }

void Light::SetConstant(float constant)					{ m_constant = constant; }
void Light::SetLinear(float linear)						{ m_linear = linear; }
void Light::SetQuadratic(float quadratic)				{ m_quadratic = quadratic; }

void Light::SetCutOff(float cutoff)						{ m_cutOff = glm::cos(glm::radians(cutoff)); }
void Light::SetOuterCutOff(float outerCutoff)			{ m_outerCutOff = glm::cos(glm::radians(outerCutoff)); }
void Light::SetMargin(float margin)						{ m_margin = margin;}

void Light::SetEnabled(bool isEnabled)					{ m_isEnabled = isEnabled; }


/*******************************************************************************************************************
	Operator overload functions
*******************************************************************************************************************/
bool Light::operator==(const Light& other) const
{
	return	m_position		== other.m_position &&
			m_direction		== other.m_direction &&
			m_ambient		== other.m_ambient &&
			m_diffuse		== other.m_diffuse &&
			m_specular		== other.m_specular &&
			m_type			== other.m_type &&
			m_constant		== other.m_constant &&
			m_linear		== other.m_linear &&
			m_quadratic		== other.m_quadratic &&
			m_cutOff		== other.m_cutOff &&
			m_outerCutOff	== other.m_outerCutOff &&
			m_isEnabled		== other.m_isEnabled &&
			m_margin		== other.m_margin;
}