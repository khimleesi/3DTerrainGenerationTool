#pragma once

/*******************************************************************************************************************
	Quad.h, Quad.cpp
	Created by Kim Kane
	Last updated: 21/03/2018
	Class finalized: 02/04/2018

	A really simple class that generates a 2D quad, useful for buttons/backgrounds, animations and particles.

	[Features]
	Generated using triangle strips - only need 4 vertices for these.
	Supports animations.

	[Upcoming]
	Nothing.

	[Side Notes]
	All primitive objects should be created within a game object/entity or interface object.
	All they are is a shape - nothing more, and so they do not have a position in the world (no transform).
	The tag they use is the tag of the object they are contained within.
	Rendering them as a standalone will result in them always being at the origin.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include "Primitive.h"

class Quad : public Primitive {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(cereal::virtual_base_class<Primitive>(this));
	}

public:
	Quad(const std::string& tag);
	Quad() {}

	virtual ~Quad();

public:
	bool Create(const std::string& tag);

public:
	virtual void Render() override;
};