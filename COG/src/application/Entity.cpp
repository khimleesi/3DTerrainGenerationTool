#include "Entity.h"
#include "graphics/shaders/EntityShader.h"
#include "utilities/Tools.h"
#include "managers/ReaderManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Entity::Entity(const std::string& tag, const Transform& transform, const Material& material, const Model& model, bool hasCollisionResponse)
	:	GameObject(tag, transform),
		m_material(material),
		m_model(model),
		m_hasCollisionResponse(hasCollisionResponse),
		m_bound(transform.GetPosition(), model.GetDimension(), transform.GetScale(), true)
{

}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Entity::~Entity()
{

}


/*******************************************************************************************************************
	A function that updates the entity
*******************************************************************************************************************/
void Entity::Update()
{
	if (m_isActive) {

		m_transform.Update();
		m_bound.Update(m_transform.GetPosition(), m_model.GetDimension(), m_transform.GetScale());
	}
	else { m_bound.Disable(); }
}


/*******************************************************************************************************************
	A function that renders the entity to the screen
*******************************************************************************************************************/
void Entity::Render(Shader* shader)
{
	if (m_isActive) {

		if (EntityShader* entityShader = Downcast<EntityShader>(shader)) {

			entityShader->SetInstanceData(&m_transform, &m_material);

			m_material.Bind();
			m_model.Render();
			m_material.Unbind();
		}
	}
}


/*******************************************************************************************************************
	A static function that creates an entity instance based on data from a file
*******************************************************************************************************************/
Entity* Entity::Create(const std::string& tag)
{
	Reader::Instance()->OpenForReading("Assets\\Files\\gameObjects.config");

	std::map<std::string, std::map<std::string, std::string>> data;

	Reader::Instance()->GetObjectData(tag, data);

	glm::vec3 position	= glm::vec3(StringToFloat(data[tag]["transform.x"]), StringToFloat(data[tag]["transform.y"]), StringToFloat(data[tag]["transform.z"]));
	glm::vec3 rotation	= glm::vec3(StringToFloat(data[tag]["rotation.x"]), StringToFloat(data[tag]["rotation.y"]), StringToFloat(data[tag]["rotation.z"]));
	glm::vec3 scale		= glm::vec3(StringToFloat(data[tag]["scale.x"]), StringToFloat(data[tag]["scale.y"]), StringToFloat(data[tag]["scale.z"]));

	bool hasCollisionResponse = StringToInteger(data[tag]["collision.response"]);

	//--- NOTE
	// This function was made for ease of use/quickness and is not optimized at all, so please ignore (I know it's ugly!)
	// Later I plan on tidying up file handling and building a proper file parsing system (I never got round to this!)
	//---

	return new Entity(
		data[tag]["tag"], Transform(position, rotation, scale),
		Material(data[tag]["material.diffuse"], data[tag]["material.normal"], data[tag]["material.specular"], data[tag]["material.emissive"]),
		Model(data[tag]["model"]), hasCollisionResponse);
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
Material* Entity::GetMaterial()				{ return &m_material; }
Model* Entity::GetModel()					{ return &m_model; }
const AABounds3D& Entity::GetBound() const	{ return m_bound; }
bool Entity::HasCollisionResponse()			{ return m_hasCollisionResponse; }