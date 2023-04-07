#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Terrain.h"
#include "utilities/Log.h"
#include "utilities/Maths.h"
#include "graphics/shaders/TerrainShader.h"
#include "managers/ResourceManager.h"
#include "utilities/Tools.h"
#include "managers/ReaderManager.h"
#include "managers/InterfaceManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set all default values of variables
*******************************************************************************************************************/
Terrain::Terrain()
	:	m_grid({ 0, 0, 0.0f, 0.0f }),
		m_width(0),
		m_height(0),
		m_level(15.0f),
		m_minimapMode(false),
		m_bounds({ { -70.0f, 0.0f, -208.0f }, { 70.0f, 0.0f, -45.0f} })
{
	
}

/*******************************************************************************************************************
	Default destructor
*******************************************************************************************************************/
Terrain::~Terrain()
{

}

/*******************************************************************************************************************
	Saves a raw heightmap file as a terrain binary file
*******************************************************************************************************************/
bool Terrain::SaveRawHeightMapData(const std::string& tag, const Transform& transform, const TexturePack& textures, const TexturePack& normals,
									const std::string& heightMapFilename, const WorldBounds& bounds, float level)
{
	m_transform = transform;
	m_textures = textures;
	m_normals = normals;
	m_bounds = bounds;
	m_level = level;
	m_heightMapFilename = heightMapFilename;

	//--- Generate the heightmap for the terrain
	if (!GenerateRawHeightMap()) { return false; }

	//--- Level out the heightmap so that the height of the terrain is not too high
	LevelHeightMap();

	//--- Calculate normals for terrain lighting (make sure this is done after leveling the terrain)
	CalculateNormals();

	//--- Calculate the terrain grid length
	m_grid.length = (float)(m_heights.size() - 1);

	//--- Determine the grids square size. Will always be 1 in this case
	m_grid.square = (float)(m_width - 1) / m_grid.length;

	//--- Flip the blend map texture
	m_textures.GetBlendMap()->SetMirrored(true);
	
	m_tag = tag;
	
	if (!PushDataToGPU()) { return false; }

	return File::Instance()->Save("Assets\\Terrain\\Binaries\\" + m_tag + ".bin",
		m_tag, m_transform, m_heightMapFilename, m_width, m_height, m_level, m_minimapMode, m_textures, m_normals, m_bounds, m_grid, m_map, m_heights);
}

/*******************************************************************************************************************
	Saves terrain binary via windows dialog
*******************************************************************************************************************/
bool Terrain::SaveTerrainViaDialog(const std::string& tag, const Transform& transform, const TexturePack& textures, const TexturePack& normals, const WorldBounds& bounds)
{
	return File::Instance()->SaveDialog(tag, transform, m_heightMapFilename, m_width, m_height, m_level, m_minimapMode, textures, normals, bounds, m_grid, m_map, m_heights);
}


/*******************************************************************************************************************
	Loads terrain binary
*******************************************************************************************************************/
bool Terrain::LoadTerrainBinary(const std::string& tag)
{
	if (!File::Instance()->Load("Assets\\Terrain\\Binaries\\" + tag + ".bin",
		m_tag, m_transform, m_heightMapFilename, m_width, m_height, m_level, m_minimapMode, m_textures, m_normals, m_bounds, m_grid, m_map, m_heights))
	{
		return false;
	}

	m_textures.LoadDiffuseFromMap();
	m_normals.LoadNormalFromMap();
	m_transform.SetDirty(true);
	if (!PushDataToGPU()) { return false; }

	return true;
}

/*******************************************************************************************************************
	Loads terrain binary via windows dialog
*******************************************************************************************************************/
bool Terrain::LoadTerrainBinaryFromDialog()
{
	if (!File::Instance()->OpenDialog(m_tag, m_transform, m_heightMapFilename, m_width, m_height, m_level, m_minimapMode, m_textures, m_normals, m_bounds, m_grid, m_map, m_heights))
	{
		return false;
	}

	m_textures.LoadDiffuseFromMap();
	m_normals.LoadNormalFromMap();
	m_transform.SetDirty(true);
	if (!PushDataToGPU()) { return false; }

	return true;
}

/*******************************************************************************************************************
	Sends data to GPU
*******************************************************************************************************************/
bool Terrain::PushDataToGPU()
{
	//--- If we already have buffers just re-use them
	if (!Resource::Instance()->AddPackedBuffers(m_tag, false)) { return false; }

	//--- Otherwise, initialize the vertex buffer that holds the geometry for the terrain
	if (!GenerateTerrain()) { return false; }

	return true;
}

/*******************************************************************************************************************
	Function that returns the height of a given x and z position within the terrain (for collision)
*******************************************************************************************************************/
float Terrain::GetHeight(float xPosition, float zPosition, float offset)
{
	//--- Convert the object coordinate passed in, into a position relative to the terrain
	float x	= xPosition - m_transform.GetPosition().x;
	float z	= -zPosition - m_transform.GetPosition().z;

	//--- Determine which grid square the object is in
	m_grid.x = (int)std::floor(x / m_grid.square);
	m_grid.z = (int)std::floor(z / m_grid.square);

	//--- Make sure the object coordinates are within a valid grid square on the terrain, if not return the height as 0
	if (m_grid.x >= m_grid.length || m_grid.z >= m_grid.length || m_grid.x < 0 || m_grid.z < 0)
	{
		return 0.0f;
	}

	//--- Find out where on the grid square the object is located.
	//--- Get the x and z distance of object from top left corner of the grid square by using % operator,
	//--- and then divide the result by the grid square size, which will give us an x and z coordinate between 0 and 1.
	glm::vec3 object = glm::vec3(0.0f);
	object.x = (std::fmod(x, m_grid.square)) / m_grid.square;
	object.z = (std::fmod(z, m_grid.square)) / m_grid.square;
	
	//--- The grid square is made up of 2 triangles, so figure out which triangle within the grid square the object is standing on
	if (object.x <= (1 - object.z)) {

		//--- If the object is in the left triangle, use Barycentric interpolation
		//--- to figure out the height of the terrain where the object is located
		object.y = maths::Barycentric(	glm::vec3(0, m_heights[m_grid.x][m_grid.z], 0),
										glm::vec3(1, m_heights[m_grid.x + 1][m_grid.z], 0),
										glm::vec3(0, m_heights[m_grid.x][m_grid.z + 1], 1),
										glm::vec2(object.x, object.z));
	}
	else {

		//--- If the object is in the right triangle, use Barycentric interpolation 
		//--- to figure out the height of the terrain where the object is located
		object.y = maths::Barycentric(	glm::vec3(1, m_heights[m_grid.x + 1][m_grid.z], 0),
										glm::vec3(1, m_heights[m_grid.x + 1][m_grid.z + 1], 1),
										glm::vec3(0, m_heights[m_grid.x][m_grid.z + 1], 1),
										glm::vec2(object.x, object.z));
	}
	
	//--- Finally, return the calculated height, plus any additional offset provided
	//--- (add an offset for when you want objects to have an additional height, still relative to the terrain height, e.g birds!)
	return object.y + offset;
}


/*******************************************************************************************************************
	Function that loads in a grayscale heightmap image
	References:
	http://www.rastertek.com/tertut02.html
*******************************************************************************************************************/
bool Terrain::GenerateRawHeightMap()
{
	//--- NOTE
	// I use stb_image as I find it more lightweight than SDL for loading in simple heightmaps
	//---

	std::string fileLocation	= "Assets\\Terrain\\Heightmaps\\" + m_heightMapFilename + ".png";
	unsigned char* imageData	= nullptr;
	int bytesPerPixel			= s_rgbOffset;
	
	//--- Load in the heightmap file. Image must be flipped vertically, otherwise pixel data will be read in incorrectly.
	stbi_set_flip_vertically_on_load(true);
	
	//--- Note we pass in 3 as the bytesPerPixel (channel value) - we want to read only RBG values.
	//--- If we wanted transparency we would change this to 4.
	imageData = stbi_load(fileLocation.c_str(), &m_width, &m_height, &bytesPerPixel, bytesPerPixel);
	
	//--- Check the file loaded correctly.
	if (!imageData) { 
		COG_LOG("[TERRAIN] Problem loading heightmap file: ", fileLocation.c_str(), LOG_ERROR); 
		GUI::Instance()->Popup("Heightmap file doesn't exist", "The heightmap file: " + fileLocation + " doesn't exist.");
		return false;
	}
	
	//--- Make sure the heightmap file has power of 2 dimensions, e.g. 128x128, 256x256, 512x512, etc.
	if ((m_width & (m_width - 1)) != 0 || (m_height & (m_height - 1)) != 0) {
		COG_LOG("[TERRAIN] Heightmap file is not power of 2 dimensions: ", fileLocation.c_str(), LOG_ERROR);
		GUI::Instance()->Popup("Heightmap file is not power of 2 dimensions", "The heightmap file: " + fileLocation + " must be power of 2 e.g. 256 x 256 pixels.");
		return false;
	}

	COG_LOG("[TERRAIN] Heightmap file loaded successfully: ", fileLocation.c_str(), LOG_SUCCESS);

	//--- Create the structure to hold the height map data
	m_map.resize((m_width) * (m_height));
	
	//--- Create the structure to hold the height data for terrain collision checks & normal calculations
	m_heights.resize(m_width);
	
	//--- Temporary variables to aid us in storing the data correctly
	unsigned char height	= 0;
	int rgb					= 0;
	int index				= 0;

	//--- Loop through the heightmap pixels
	for (int row = 0; row < m_height; row++) {
		for (int column = 0; column < m_width; column++) {

			//--- Set the height variable to the RGB pixel value read in from the heightmap file
			height = imageData[rgb];
	
			//--- Create the structure to hold the height data for terrain collision checks & normal calculations
			m_heights[column].resize(m_width);
	
			//--- Store the height of the terrain at this point into the heights container
			//--- so we can access it later to determine collision and normals
			m_heights[column][row] = height;
	
			//--- Move through the heightmap file and increment the index each time
			index = (m_height * row) + column;

			//--- Finally, set the heightmap data to the values calculated; y being the pixel data read
			//--- in from the heightmap image, and x and z being the incrementation of our nested for loops (0 - width, 0 - height)
			m_map[index].position.x = (float)column;
			m_map[index].position.y = (float)height;
			m_map[index].position.z = (float)row;

			m_map[index].textureCoord.s = (float)column;
			m_map[index].textureCoord.t = (float)row;

			//--- We only need to read in the 'r' (red) value, as RGB will be the same colour values, 
			//--- due to it being a grayscale image. So, we increment by 3, skipping the green and blue colour values.
			rgb += bytesPerPixel;
		}
	}

	//--- Delete the image data now we have it stored in our containers
	if (imageData) {
		stbi_image_free(imageData);
	}

	//--- NOTE 
	// m_map struct could also have an 'a' float that we could use to store the alpha value,
	// then do some fancy stuff with transparency later! (floating terrain/cliffs/black holes!)
	// Also note that we don't currently store the terrain anywhere once it has been loaded in
	// Every time a new game state is made, the terrain height map is loaded and the heightmap data is re-generated
	// In future, we probably want to store this data somewhere in memory so we can just reference previous
	// heightmap data, rather that loading it every time. Or, we could have a game object cache/object pool of some sort
	//---

	return true;
}


/*******************************************************************************************************************
	Function that calculates terrain normals, using finite difference method 
	References:
	https://en.wikipedia.org/wiki/Finite_difference_method
	https://www.youtube.com/watch?v=O9v6olrHPwI&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=21
*******************************************************************************************************************/
void Terrain::CalculateNormals()
{
	int index = 0;

	//--- Neighbouring vertices - left, right, bottom and top
	struct { float l, r, b, t; } neighbours = { 0 };
	
	for (int row = 0; row < m_height; row++) {
		for (int column = 0; column < m_width; column++) {

			index = (m_height * row) + column;

			//--- We calculate the height of all 4 neighbouring vertices
			neighbours.l = FindHeightAtPoint(column - 1, row);
			neighbours.r = FindHeightAtPoint(column + 1, row);
			neighbours.b = FindHeightAtPoint(column, row - 1);
			neighbours.t = FindHeightAtPoint(column, row + 1);

			//--- Then create the normal from the data generated above
			glm::vec3 normal = glm::normalize(glm::vec3(neighbours.l - neighbours.r, 2.0f, neighbours.b - neighbours.t));
			
			m_map[index].normal = normal;
		}
	}
}


/*******************************************************************************************************************
	Function that returns the height of the terrain at a specific point
*******************************************************************************************************************/
float Terrain::FindHeightAtPoint(int column, int row)
{
	if (column < 0)			{ column = 0; }
	if (row < 0)			{ row = 0; }
	if (column >= m_width)	{ column = (m_width - 1); }
	if (row >= m_height)	{ row = (m_height - 1); }

	return m_heights[column][row];
}


/*******************************************************************************************************************
	Function that levels out a heightmap (shrinks/expands the height/width/depth of terrain)
*******************************************************************************************************************/
void Terrain::LevelHeightMap()
{
	for (int row = 0; row < m_height; row++) {
		for (int column = 0; column < m_width; column++) {
			m_map[(m_height * row) + column].position.y	/= m_level;
			m_heights[column][row]						/= m_level;
		}
	}
}


/*******************************************************************************************************************
	Function that generates the terrain vertex positions, prior to sending the data to GPU for rendering
*******************************************************************************************************************/
bool Terrain::GenerateTerrain()
{
	//--- We do -1 to make the width and height of terrain an odd number, necessary for accurate placement of vertex data
	int offsetHeight	= (m_height - 1);
	int offsetWidth		= (m_width - 1);

	//--- Calculate number of vertices in terrain mesh (6 points/vertices to make 1 face - 2 x triangles, 3 points per triangle)
	unsigned int vertexCount = (offsetHeight) * (offsetWidth) * s_vertexCount;

	//--- Resize vector to the size of the vertex count
	std::vector<VertexBuffer::PackedVertex> vertices(vertexCount);

	unsigned int index	= 0;
	
	//--- Vertex positions for each vertex - bottom left, bottom right, top left and top right
	struct { int bottomLeft, bottomRight, topLeft, topRight; } vertex = { 0 };

	//--- Temporary variables for tangent and bitangent calculations
	struct { glm::vec3 first, second; } deltaPosition = {};
	struct { glm::vec2 first, second; } deltaTexCoord = {};
	
	float denominator	= 0.0f;
	
	glm::vec3 tangent	= glm::vec3(0.0f);
	glm::vec3 bitangent = glm::vec3(0.0f);

	//--- Loop through the terrain and place vertices in relevant positions, adding the data to the vertices vector
	for (int row = 0; row < offsetHeight; row++) {
		for (int column = 0; column < offsetWidth; column++) {
			
			//--- Calculate the vertex positions on the screen
			vertex.bottomLeft	= (m_height * row) + column;
			vertex.bottomRight	= (m_height * row) + (column + 1);
			vertex.topLeft		= (m_height * (row + 1)) + column;
			vertex.topRight		= (m_height * (row + 1)) + (column + 1);

			//--- Ugly maths formula's to get the tangent and bitangent of the terrain for normal mapping.
			//--- References: 
			//--- http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
			//--- https://learnopengl.com/Advanced-Lighting/Normal-Mapping
			//--- This could be made a lot cleaner in future - but I kept it simple so that we can see how the terrain is generated
			
			//--- First Triangle
			deltaPosition.first		= m_map[vertex.topLeft].position - m_map[vertex.topRight].position;
			deltaPosition.second	= m_map[vertex.bottomLeft].position - m_map[vertex.topRight].position;
			deltaTexCoord.first		= m_map[vertex.topLeft].textureCoord - m_map[vertex.topRight].textureCoord;
			deltaTexCoord.second	= m_map[vertex.bottomLeft].textureCoord - m_map[vertex.topRight].textureCoord;

			denominator	= 1.0f / (deltaTexCoord.first.x * deltaTexCoord.second.y - deltaTexCoord.second.x * deltaTexCoord.first.y);
			tangent		= denominator * (deltaTexCoord.second.y * deltaPosition.first - deltaTexCoord.first.y * deltaPosition.second);
			bitangent	= denominator * (-deltaTexCoord.second.x * deltaPosition.first + deltaTexCoord.first.x * deltaPosition.second);
			
			glm::normalize(tangent);
			glm::normalize(bitangent);

			vertices[index].position		= m_map[vertex.topRight].position;
			vertices[index].textureCoord	= m_map[vertex.topRight].textureCoord;
			vertices[index].normal			= m_map[vertex.topRight].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;

			vertices[index].position		= m_map[vertex.topLeft].position;
			vertices[index].textureCoord	= m_map[vertex.topLeft].textureCoord;
			vertices[index].normal			= m_map[vertex.topLeft].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;

			vertices[index].position		= m_map[vertex.bottomLeft].position;
			vertices[index].textureCoord	= m_map[vertex.bottomLeft].textureCoord;
			vertices[index].normal			= m_map[vertex.bottomLeft].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;

			//--- Second Triangle
			deltaPosition.first		= m_map[vertex.bottomRight].position - m_map[vertex.bottomLeft].position;
			deltaPosition.second	= m_map[vertex.topRight].position - m_map[vertex.bottomLeft].position;
			deltaTexCoord.first		= m_map[vertex.bottomRight].textureCoord - m_map[vertex.bottomLeft].textureCoord;
			deltaTexCoord.second	= m_map[vertex.topRight].textureCoord - m_map[vertex.bottomLeft].textureCoord;

			denominator = 1.0f / (deltaTexCoord.first.x * deltaTexCoord.second.y - deltaTexCoord.second.x * deltaTexCoord.first.y);
			tangent		= denominator * (deltaTexCoord.second.y * deltaPosition.first - deltaTexCoord.first.y * deltaPosition.second);
			bitangent	= denominator * (-deltaTexCoord.second.x * deltaPosition.first + deltaTexCoord.first.x * deltaPosition.second);
			
			glm::normalize(tangent);
			glm::normalize(bitangent);

			vertices[index].position		= m_map[vertex.bottomLeft].position;
			vertices[index].textureCoord	= m_map[vertex.bottomLeft].textureCoord;
			vertices[index].normal			= m_map[vertex.bottomLeft].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;

			vertices[index].position		= m_map[vertex.bottomRight].position;
			vertices[index].textureCoord	= m_map[vertex.bottomRight].textureCoord;
			vertices[index].normal			= m_map[vertex.bottomRight].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;

			vertices[index].position		= m_map[vertex.topRight].position;
			vertices[index].textureCoord	= m_map[vertex.topRight].textureCoord;
			vertices[index].normal			= m_map[vertex.topRight].normal;
			vertices[index].tangent			= tangent;
			vertices[index].bitangent		= bitangent;
			index++;
		}
	}


	//--- NOTE
	// The above generation of vertex data was previously encapsulated into one function
	// However I have since wrote it out in its plainest form to make it easier to see what is happening
	// The code looks ugly imo! But it is easier to understand this way, for those who haven't used heightmaps before
	// Hint: Render the terrain in wireframe mode to see some magic happening ;)
	//---

	//--- Push the vertex data to the GPU for rendering	(hurrah)
	Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetPackedVBO(m_tag)->Push(vertices, false);
	Resource::Instance()->GetVAO(m_tag)->Unbind();

	return true;
}


/*******************************************************************************************************************
	Function that updates the terrain providing any changes have happened
*******************************************************************************************************************/
void Terrain::Update()
{
	m_transform.Update();
}


/*******************************************************************************************************************
	Function that renders the terrain to the screen
*******************************************************************************************************************/
void Terrain::Render(Shader* shader)
{
	if (TerrainShader* terrainShader = Downcast<TerrainShader>(shader)) {

		terrainShader->SetInstanceData(&m_transform, m_textures.GetBlendMap(), m_minimapMode);

		m_textures.Bind();
		m_normals.Bind();

		Resource::Instance()->GetVAO(m_tag)->Bind();
		Resource::Instance()->GetPackedVBO(m_tag)->Render();

		m_normals.Unbind();
		m_textures.Unbind();
	}
}


/*******************************************************************************************************************
	Accessor Methods
*******************************************************************************************************************/
Terrain::TerrainGrid* Terrain::GetGrid()		{ return &m_grid; }
Terrain::WorldBounds* Terrain::GetBounds()		{ return &m_bounds; }
bool Terrain::IsMinimapEnabled()				{ return m_minimapMode; }
void Terrain::SetMinimapMode(bool minimapMode)	{ m_minimapMode = minimapMode; }


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
const unsigned int Terrain::s_rgbOffset		= 3;
const unsigned int Terrain::s_vertexCount	= 6;
const unsigned int Terrain::s_maxTextures	= 5;
const unsigned int Terrain::s_maxNormalMaps	= 4;

const unsigned int Terrain::GetMaxTextures()		{ return s_maxTextures; }
const unsigned int Terrain::GetMaxNormalMaps()		{ return s_maxNormalMaps; }