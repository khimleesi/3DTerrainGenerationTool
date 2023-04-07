#pragma once

/*******************************************************************************************************************
	ObjLoader.h, ObjLoader.cpp
	Created by Kim Kane
	Last updated: 14/03/2018
	Class finalized: 14/03/2018

	Loads in an OBJ file.
	References: OpenGL Insights, by Patrick Cozzi and Cristophe Riccio

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include "graphics/buffers/VertexBuffer.h"

class ObjLoader {

public:
	ObjLoader();
	~ObjLoader();

public:
	bool LoadObjFile(const std::string& obj, std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outTextureCoords, std::vector<glm::vec3>& outNormals, std::vector<unsigned int>* outIndices = nullptr);

private:
	void GenerateIndexedObject(std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outTextureCoords, std::vector<glm::vec3>& outNormals, std::vector<unsigned int>& outIndices);
	bool GetSimilarVertexIndex(VertexBuffer::PackedVertex& packed, std::map<VertexBuffer::PackedVertex, unsigned int>& vertexToOutIndex, unsigned int& result);
	
private:
	void GetVertices(std::vector<glm::vec3>& inVertices);
	void GetTextureCoords(std::vector<glm::vec2>& inTextureCoords);
	void GetNormals(std::vector<glm::vec3>& inNormals);
	void GetIndices(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& textureCoordIndices, std::vector<unsigned int>& normalIndices);

private:
	template <typename T> void CalibrateIndices(std::vector<T>& inData, std::vector<T>& finalData, std::vector<unsigned int>& finalIndices);

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_textureCoords;
	std::vector<glm::vec3> m_normals;
};

/*******************************************************************************************************************
	Template function that calibrates/adjusts all indices retrieved from an OBJ file, so they are correctly stored
*******************************************************************************************************************/
template <typename T> void ObjLoader::CalibrateIndices(std::vector<T>& inData, std::vector<T>& finalData, std::vector<unsigned int>& finalIndices)
{
	//--- We do -1, as the vertices from an obj file do not start from 0, they start from position 1. 
	//--- Where-as array's in C++ start from 0
	//--- But we also need to check that the actual indices in the file aren't zero,
	//--- and if they are set them to 1, so then we access tempVertices[0]
	//--- (Otherwise we would be accessing tempVertices[-1], which clearly breaks EVERYTHING
	for (unsigned int i = 0; i < finalIndices.size(); i++) {
		if (finalIndices[i] == 0) { finalIndices[i] = 1; }
		finalData.push_back(inData[finalIndices[i] - 1]);
	}
}