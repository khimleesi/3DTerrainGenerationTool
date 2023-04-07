#include "ObjLoader.h"
#include "managers/ReaderManager.h"
#include "utilities/Log.h"
#include "utilities/Tools.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
ObjLoader::ObjLoader()
{
	COG_LOG("[OBJ LOADER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Cleanup all memory usage, delete all objects and shut down all devices
*******************************************************************************************************************/
ObjLoader::~ObjLoader()
{
	COG_LOG("[OBJ LOADER DESTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Load an OBJ file and store the contents into the vector containers passed in
*******************************************************************************************************************/
bool ObjLoader::LoadObjFile(const std::string& obj, std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outTextureCoords, std::vector<glm::vec3>& outNormals, std::vector<unsigned int>* outIndices)
{
	//--- Open the OBJ file for reading only
	if (!Reader::Instance()->OpenForReading(obj.c_str())) { return false; }

	//--- Generate temporary vectors to store the faces/indices data
	std::vector<unsigned int> vertexIndices, textureCoordIndices, normalIndices;

	//--- Generate temporary vectors to store the other obj data
	std::vector<glm::vec3> inVertices;
	std::vector<glm::vec2> inTextureCoords;
	std::vector<glm::vec3> inNormals;

	using namespace file_constants;
	//--- Get all the data from the file
	while (Reader::Instance()->ExtractFileData()) {

		//--- Check if the file contains "v", "vt", or "vn", remove this part of the string and read in the object data only
		if		(Reader::Instance()->FileDataContains(VERTICES))			{ GetVertices(inVertices); }
		else if (Reader::Instance()->FileDataContains(TEXTURE_COORDS))		{ GetTextureCoords(inTextureCoords); }
		else if (Reader::Instance()->FileDataContains(NORMALS))			{ GetNormals(inNormals); }
		else if (Reader::Instance()->FileDataContains(FACES))				{ GetIndices(vertexIndices, textureCoordIndices, normalIndices); }
	}

	//--- We then need to calibrate the indices (-1 all indices) before we push the data in to the m_vertices,
	//--- m_textureCoords and m_normals vectors, because arrays in C++ start from 0, and OBJ files start from 1
	CalibrateIndices(inVertices, m_vertices, vertexIndices);
	CalibrateIndices(inTextureCoords, m_textureCoords, textureCoordIndices);
	CalibrateIndices(inNormals, m_normals, normalIndices);

	//--- If we want to draw elements index-based (to avoid duplicated vertex data), we then call this function,
	//--- which checks for multiple vertex data and then finally pushes the data to the out vectors
	if (outIndices) { GenerateIndexedObject(outVertices, outTextureCoords, outNormals, *outIndices); }
	
	//--- If we want to draw just the vertices as they are, then we push the data from the OBJ file into
	//--- the vectors passed in for plain vertex buffer drawing
	else {
		for (unsigned int i = 0; i < m_vertices.size(); i++) {
			outVertices.emplace_back(m_vertices[i]);
			outTextureCoords.emplace_back(m_textureCoords[i]);
			outNormals.emplace_back(m_normals[i]);
		}
	}

	//--- Close the file once we are finished with it.
	//--- This is necessary as other OBJ files cannot be loaded as long as the file remains open
	Reader::Instance()->Close(obj.c_str());

	return true;
}


/*******************************************************************************************************************
	Function that finalizes the file data, disregards duplicate vertices and pushes it to the vectors passed in
*******************************************************************************************************************/
void ObjLoader::GenerateIndexedObject(std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outTextureCoords, std::vector<glm::vec3>& outNormals, std::vector<unsigned int>& outIndices)
{
	std::map<VertexBuffer::PackedVertex, unsigned int> vertexToOutIndex;

	//--- For each vertex stored in our m_vertices vector
	for (unsigned int i = 0; i < m_vertices.size(); i++) {

		VertexBuffer::PackedVertex packed = { m_vertices[i], m_textureCoords[i], m_normals[i] };

		//--- Try to find a similar vertex in our vectors
		unsigned int index = 0;
		bool found = GetSimilarVertexIndex(packed, vertexToOutIndex, index);

		//--- If we find a similar vertex, use that instead
		if (found) { outIndices.emplace_back(index); }

		//--- If not, push the data to the vectors passed in
		else {
			outVertices.emplace_back(m_vertices[i]);
			outTextureCoords.emplace_back(m_textureCoords[i]);
			outNormals.emplace_back(m_normals[i]);
			unsigned int newIndex = (unsigned int)outVertices.size() - 1;
			outIndices.emplace_back(newIndex);
			vertexToOutIndex[packed] = newIndex;
		}
	}

}


/*******************************************************************************************************************
	Function that checks if we have a similar vertex already stored
*******************************************************************************************************************/
bool ObjLoader::GetSimilarVertexIndex(VertexBuffer::PackedVertex& packed, std::map<VertexBuffer::PackedVertex, unsigned int>& vertexToOutIndex, unsigned int& result)
{
	//--- Loop through the map, looking for a vertex
	std::map<VertexBuffer::PackedVertex, unsigned int>::iterator it = vertexToOutIndex.find(packed);

	//--- If vertex is not found in the map, then we need to add it in the GenerateIndexedObject function, so return false
	if (it == vertexToOutIndex.end()) { return false; }

	//--- Otherwise, if the vertex is found in the map, then we just return this vertex and don't create another one
	else { result = it->second; }

	return true;
}


/*******************************************************************************************************************
	Function that gets all the vertices from the OBJ file and stores them in to the vector passed in
*******************************************************************************************************************/
void ObjLoader::GetVertices(std::vector<glm::vec3>& inVertices)
{
	glm::vec3 vertex(0.0f);

	//--- Disregard the "v" and the space after it, and store this new string in to a temporary string
	std::string vertices = Reader::Instance()->CutFileData(file_constants::VERTICES_OFFSET);

	//--- Store the temporary string we just created (which now only contains the vertices) in to the fileData stream
	std::stringstream fileData(vertices);

	//--- Add x, y and z data in our fileData to the x, y and z of our vertex
	fileData >> vertex.x >> vertex.y >> vertex.z;

	//--- Add all of the vertex data from the file to our temporary vertices vector
	inVertices.emplace_back(vertex);
}


/*******************************************************************************************************************
	Function that gets all the texture coordinates from the OBJ file and stores them in to the vector passed in
*******************************************************************************************************************/
void ObjLoader::GetTextureCoords(std::vector<glm::vec2>& inTextureCoords)
{
	glm::vec2 textureCoord(0.0f);

	//--- Disregard the "vt" and the space after it, and store this new string in to a temporary string
	std::string textureCoords = Reader::Instance()->CutFileData(file_constants::TEXTURE_COORDS_OFFSET);

	//--- Store the temporary string we just created (which now only contains the texture coordinates) in to the fileData stream
	std::stringstream fileData(textureCoords);

	//--- Add s(x) and t(y) data in our fileData to the s and t of our texture coordinate
	fileData >> textureCoord.s >> textureCoord.t;

	//--- Add all of the texture coordinate data from the file to our temporary texture coords vector
	inTextureCoords.emplace_back(textureCoord);
}


/*******************************************************************************************************************
	Function that gets all the normals from the OBJ file and stores them in to the vector passed in
*******************************************************************************************************************/
void ObjLoader::GetNormals(std::vector<glm::vec3>& inNormals)
{
	glm::vec3 normal(0.0f);

	//--- Disregard the "vn" and the space after it, and store this new string in to a temporary string
	std::string normals = Reader::Instance()->CutFileData(file_constants::NORMALS_OFFSET);

	//--- Store the temporary string we just created (which now only contains the normals) in to the fileData stream
	std::stringstream fileData(normals);

	//--- Add x, y and z data in our fileData to the x, y and z of our normal
	fileData >> normal.x >> normal.y >> normal.z;

	//--- Add all of the normal data from the file to our temporary normals vector
	inNormals.emplace_back(normal);
}


/*******************************************************************************************************************
	Function that gets all the indices from the OBJ file and stores them in to the vectors passed in
*******************************************************************************************************************/
void ObjLoader::GetIndices(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& textureCoordIndices, std::vector<unsigned int>& normalIndices)
{
	glm::uvec3 vertexIndex(0), textureCoordIndex(0), normalIndex(0);
	
	std::string faces = Reader::Instance()->CutFileData(file_constants::FACES_OFFSET);

	//--- We replace the "/" with a space, so that the stream reads in every value before or after the space
	//--- (quick fix - there's better ways of doing this)
	ReplaceCharacter(faces, '/', ' ');
	
	std::stringstream fileData(faces);

	//--- Put all the file data in to relevant x, y and z
	fileData	>> vertexIndex.x >> textureCoordIndex.x >> normalIndex.x
				>> vertexIndex.y >> textureCoordIndex.y >> normalIndex.y
				>> vertexIndex.z >> textureCoordIndex.z >> normalIndex.z;

	//--- Loop through all the index values obtained above, and store them in to our faces (indices) vectors
	for (int i = 0; i < vertexIndex.length(); i++) { 
		vertexIndices.emplace_back(vertexIndex[i]);
		textureCoordIndices.emplace_back(textureCoordIndex[i]);
		normalIndices.emplace_back(normalIndex[i]);
	}
}