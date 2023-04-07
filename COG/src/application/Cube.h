#pragma once

/*******************************************************************************************************************
	Cube.h, Cube.cpp
	Created by Kim Kane
	Last updated: 21/03/2018
	Class finalized: 02/04/2018

	A really simple class that generates a 3D cube, useful for light boxes (for testing lights) and sky boxes.

	[Features]
	Set up for the default CCW drawing and supports culling of the back face.
	Supports indexed rendering.

	[Upcoming]
	Nothing.

	[Side Notes]
	All primitive objects should be created within a game object/entity or interface object.
	All they are is a shape - nothing more, and so they do not have a position in the world (no transform).
	The tag they use is the tag of the object they are contained within.
	Rendering them as a standalone will result in them always being at the origin.

*******************************************************************************************************************/
#include "Primitive.h"

class Cube : public Primitive {

public:
	Cube(const std::string& tag);
	virtual ~Cube();

public:
	bool Create(float size);

public:
	virtual void Render() override;
};