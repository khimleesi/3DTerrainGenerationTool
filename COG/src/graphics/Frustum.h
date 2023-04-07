#pragma once

/*******************************************************************************************************************
	Frustum.h, Frustum.cpp
	Created by Kim Kane
	Last updated: 19/05/2018

	Generates a culling frustum to cull objects not visible by the projection.

	[Features]
	Optimizes rendering of many objects.
	Support for points, spheres, boxes and rectangles.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	References and Credits:
	Ben Humphrey (DigiBen)
	Game Programmer
	DigiBen@GameTutorials.com
	o-Web Host of www.GameTutorials.com
	https://gist.github.com/jimmikaelkael/2e4ffa5712d61816c7ca

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include <vector>

class Frustum {

private:
	//--- Position of plane's normal, and the distance the plane is from the origin
	enum PlaneType { A, B, C, D };

	//--- Sides of the viewing frustum
	enum SideType { RIGHT, LEFT, BOTTOM, TOP, BACK, FRONT };

public:
	Frustum(const glm::mat4& projection, const glm::mat4& view);
	~Frustum();

public:
	void Update(const glm::mat4& projection, const glm::mat4& view);
	
public:
	bool IsPointInside(const glm::vec3& position);
	bool IsSphereInside(const glm::vec3& centerPosition, float radius);
	bool IsCubeInside(const glm::vec3& centerPosition, float halfDepth);
	bool IsRectangleInside(const glm::vec3& centerPosition, const glm::vec3& halfDimension);

private:
	void NormalizePlane(std::vector <std::vector<float>>& frustum, SideType side);

private:
	std::vector<std::vector<float>> m_frustum;

private:
	static const unsigned int s_maxPlaneContents;
	static const unsigned int s_maxSides;
};