#pragma once

/*******************************************************************************************************************
	Terrain.h, Terrain.cpp
	Created by Kim Kane
	Last updated: 17/03/2018

	Terrain class that loads in pixel data from a heightmap file, allowing open-world multi-height terrain generation.
	
	[Features]
	Collision using barycentric coordinates.
	Terrain mesh transformations/rotations and scaling (for OpenGL, make the z coordinate of the scale -1).
	Multi-textures.
	Transparency (easy to grab the alpha channel from the height map data).
	2D grid implementation, useful for trigger points/spawn locations/grid collisions.
	Normal generation using finite difference method (good for lighting!)
	Tangent and bitangent support for normal mapping.

	[Upcoming]
	Indexed rendering of terrain mesh.
	Terrain will be a complete mesh in future using a PackedVertex struct like every other mesh.
	Tangents and bitangents will be calculated elsewhere.
	OBJ parser allowing us to save the terrain mesh we generated to an obj file & then load in binary form (faster load times).
	Terrain will be cached in memory - allowing re-use of an already generated heightmap.

	[Side Notes]
	A heightmap file is a grayscale image of RBG color values, all of which are the same.
	An alpha channel is not necessary (unless you want to mimic holes or semi-transparent illusions in your terrain).
	Due to this, I use PNG files for the heightmap, but ignore the alpha channel.
	In future, if we decided to have transparency, this would be simple to implement.

	This class is a forever on-going project of mine (I will need it for my dissertation)
	and it will continue being updated constantly. It is impossible to do the work in the time frame 
	I had to finish this project - for now it does what I need it to do and any further work/amendments will be done another day!

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include <string>
#include <vector>
#include "application/GameObject.h"
#include "graphics/TexturePack.h"

class Terrain : public GameObject {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(cereal::virtual_base_class<GameObject>(this),
				COG_NVP(m_heightMapFilename),
				COG_NVP(m_width),
				COG_NVP(m_height),
				COG_NVP(m_level),
				COG_NVP(m_minimapMode),
				COG_NVP(m_textures),
				COG_NVP(m_normals),
				COG_NVP(m_bounds),
				COG_NVP(m_grid),
				COG_NVP(m_map),
				COG_NVP(m_heights)
			);
	}

private:
	struct HeightMap {
		glm::vec3 position;
		glm::vec2 textureCoord;
		glm::vec3 normal;

		template <class Archive>
		void Serialize(Archive& archive)
		{
			archive(COG_NVP(position), COG_NVP(textureCoord), COG_NVP(normal));
		}
	};

	struct TerrainGrid {
		int x, z;
		float length;
		float square;

		template <class Archive>
		void Serialize(Archive& archive)
		{
			archive(COG_NVP(x), COG_NVP(z), COG_NVP(length), COG_NVP(square));
		}
	};

	struct WorldBounds {
		glm::vec3 minimum, maximum;

		template <class Archive>
		void Serialize(Archive& archive)
		{
			archive(COG_NVP(minimum), COG_NVP(maximum));
		}
	};

public:
	Terrain();
	virtual ~Terrain();

public:
	virtual void Update()				override;
	virtual void Render(Shader* shader) override;
	
public:
	bool SaveRawHeightMapData(const std::string& tag, const Transform& transform, const TexturePack& textures, const TexturePack& normals,
							  const std::string& heightMapFilename, const WorldBounds& bounds, float level = 25.0f);
	bool SaveTerrainViaDialog(const std::string& tag, const Transform& transform, const TexturePack& textures, const TexturePack& normals, const WorldBounds& bounds);
	bool LoadTerrainBinary(const std::string& tag);
	bool LoadTerrainBinaryFromDialog();

public:
	float			GetHeight(float xPosition, float zPosition, float offset = 0.0f);
	TerrainGrid*	GetGrid();
	WorldBounds*	GetBounds();
	TexturePack*	GetDiffuseTexturePack() { return &m_textures; }
	TexturePack*	GetNormalTexturePack() { return &m_normals; }

public:
	inline void SetBounds(const glm::vec3& minimum, const glm::vec3& maximum) { m_bounds.minimum = minimum; m_bounds.maximum = maximum; }
	void SetMinimapMode(bool minimapMode);
	bool IsMinimapEnabled();

public:
	static const unsigned int GetMaxTextures();
	static const unsigned int GetMaxNormalMaps();
	
private:
	bool GenerateRawHeightMap();
	void LevelHeightMap();
	void CalculateNormals();
	bool GenerateTerrain();
	bool PushDataToGPU();

private:
	float FindHeightAtPoint(int column, int row);
	
private:
	std::string m_heightMapFilename;
	int		m_width, m_height;
	float	m_level;
	bool	m_minimapMode;

private:
	TerrainGrid m_grid;
	TexturePack	m_textures;
	TexturePack	m_normals;
	WorldBounds m_bounds;

private:
	std::vector<HeightMap>			m_map;
	std::vector<std::vector<float>> m_heights;

private:
	static const unsigned int s_maxTextures;
	static const unsigned int s_maxNormalMaps;
	static const unsigned int s_rgbOffset;
	static const unsigned int s_vertexCount;
};