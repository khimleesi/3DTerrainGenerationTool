#include <pretty_assimp/Importer.hpp>
#include <pretty_assimp/scene.h>
#include <pretty_assimp/postprocess.h>
#include <pretty_assimp/cimport.h>
#include <algorithm>
#include "Model.h"
#include "utilities/Log.h"
#include "managers/ResourceManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
Model::Model(const std::string& obj)
	:	m_tag(obj)
{
	Load();
}


/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Model::~Model()
{

}


/*******************************************************************************************************************
	Draws the model to the screen
*******************************************************************************************************************/
void Model::Render()
{
	//--- Bind the VAO related to this model
	Resource::Instance()->GetVAO(m_tag)->Bind();

	//--- Render the EBO related to this model
	Resource::Instance()->GetEBO(m_tag)->Render();
}


/*******************************************************************************************************************
	Function that loads the object data from an OBJ file using Assimp and stores the data into the relevant vectors
*******************************************************************************************************************/
bool Model::Load()
{
	if (m_tag.empty()) { return false; }

	//--- Check if we already have a model with this tag and if so exit function and use pre-existing data
	if (!Resource::Instance()->AddPackedBuffers(m_tag, true)) { return false; }

	std::string src = "Assets\\Models\\" + m_tag;

	//--- Load in the model using Assimp
	const aiScene* scene = aiImportFile(src.c_str(),	aiProcess_Triangulate |
														aiProcess_FixInfacingNormals |
														aiProcess_CalcTangentSpace |
														aiProcess_FindInvalidData |
														aiProcess_OptimizeMeshes |
														aiProcess_OptimizeGraph |
														aiProcess_FlipUVs |
														aiProcess_ValidateDataStructure);

	//--- Check for any errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		COG_LOG("[MODEL] Error loading Assimp model: ", aiGetErrorString, LOG_ERROR);
		return false;
	}

	if (!scene->HasMeshes()) { return false; }
		
	unsigned int vertexCount = 0;

	//--- Get the vertex count by looping through the vertices of each mesh
	for (unsigned int mesh = 0; mesh < scene->mNumMeshes; mesh++)
	{
		vertexCount += scene->mMeshes[mesh]->mNumVertices;
	}

	//--- Create our temporary vectors, resizing/reserving memory as necessary
	std::vector<VertexBuffer::PackedVertex> packedVertex(vertexCount);
	std::vector<unsigned int> indices;
	indices.reserve(vertexCount);

	//--- Get all the data from the file and store this into our containers
	for (unsigned int mesh = 0; mesh < scene->mNumMeshes; mesh++)
	{
		for (unsigned int vertex = 0; vertex < scene->mMeshes[mesh]->mNumVertices; vertex++)
		{
			packedVertex[vertex].position.x		= scene->mMeshes[mesh]->mVertices[vertex].x;
			packedVertex[vertex].position.y		= scene->mMeshes[mesh]->mVertices[vertex].y;
			packedVertex[vertex].position.z		= scene->mMeshes[mesh]->mVertices[vertex].z;

			packedVertex[vertex].textureCoord.s = scene->mMeshes[mesh]->mTextureCoords[0][vertex].x;
			packedVertex[vertex].textureCoord.t = scene->mMeshes[mesh]->mTextureCoords[0][vertex].y;
	
			if (scene->mMeshes[mesh]->HasNormals()) {
				packedVertex[vertex].normal.x = scene->mMeshes[mesh]->mNormals[vertex].x;
				packedVertex[vertex].normal.y = scene->mMeshes[mesh]->mNormals[vertex].y;
				packedVertex[vertex].normal.z = scene->mMeshes[mesh]->mNormals[vertex].z;
			}

			if (scene->mMeshes[mesh]->HasTangentsAndBitangents()) {
				packedVertex[vertex].tangent.x = scene->mMeshes[mesh]->mTangents[vertex].x;
				packedVertex[vertex].tangent.y = scene->mMeshes[mesh]->mTangents[vertex].y;
				packedVertex[vertex].tangent.z = scene->mMeshes[mesh]->mTangents[vertex].z;

				packedVertex[vertex].bitangent.x = scene->mMeshes[mesh]->mBitangents[vertex].x;
				packedVertex[vertex].bitangent.y = scene->mMeshes[mesh]->mBitangents[vertex].y;
				packedVertex[vertex].bitangent.z = scene->mMeshes[mesh]->mBitangents[vertex].z;
			}
		}
		
		for (unsigned int face = 0; face < scene->mMeshes[mesh]->mNumFaces; face++)
		{
			for (unsigned int index = 0; index < scene->mMeshes[mesh]->mFaces[face].mNumIndices; index++)
			{
				indices.emplace_back(scene->mMeshes[mesh]->mFaces[face].mIndices[index]);
			}
		}
	}

	//--- Bind the VAO and push vertex and index buffer data to the GPU
	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetPackedVBO(m_tag)->Push(packedVertex, false);
		Resource::Instance()->GetEBO(m_tag)->Push(indices);
	Resource::Instance()->GetVAO(m_tag)->Unbind();

	//--- Destroy the data now the GPU has it
	aiReleaseImport(scene);

	//--- Calculate the models' width, height and depth, which we will need for any collisions
	CalculateDimension(packedVertex);

	COG_LOG("[MODEL] Assimp model created: ", m_tag.c_str(), LOG_RESOURCE);

	return true;
}


/*******************************************************************************************************************
	Function that calculates the dimensions of a 3D model, based on min/max values of its vertices
*******************************************************************************************************************/
void Model::CalculateDimension(std::vector<VertexBuffer::PackedVertex>& container)
{
	if (!container.empty()) {
	
		glm::vec3 dimension = glm::vec3(0.0f);
		
		//--- Find the min and max value of the X vertex position
		auto x = std::minmax_element(container.begin(), container.end(),
			[](const VertexBuffer::PackedVertex& first, const VertexBuffer::PackedVertex& second) {
			return first.position.x < second.position.x;
		});
		
		//--- Store the width value
		dimension.x = (glm::distance(x.first->position.x, x.second->position.x));

		//--- Find the min and max value of the Y vertex position
		auto y = std::minmax_element(container.begin(), container.end(),
			[](const VertexBuffer::PackedVertex& first, const VertexBuffer::PackedVertex& second) {
			return first.position.y < second.position.y;
		});

		//--- Store the height value
		dimension.y = (glm::distance(y.first->position.y, y.second->position.y));

		//--- Find the min and max value of the Z vertex position
		auto z = std::minmax_element(container.begin(), container.end(),
			[](const VertexBuffer::PackedVertex& first, const VertexBuffer::PackedVertex& second) {
			return first.position.z < second.position.z;
		});

		//--- Store the depth value
		dimension.z = (glm::distance(z.first->position.z, z.second->position.z));

		m_dimensions.try_emplace(m_tag, dimension);
	}
}


/*******************************************************************************************************************
	Accessor methods
*******************************************************************************************************************/
const glm::vec3& Model::GetDimension() const { return m_dimensions[m_tag]; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
std::map<std::string, glm::vec3> Model::m_dimensions;