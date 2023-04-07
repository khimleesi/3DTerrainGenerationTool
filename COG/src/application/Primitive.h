#pragma once

/*******************************************************************************************************************
	Primitive.h, Primitive.cpp
	Created by Kim Kane
	Last updated: 08/04/2018
	Class finalized: 08/04/2018

	A base class for all our in-game primitives.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	All primitive objects should be created within a game object/entity or interface object.
	All they are is a shape - nothing more, and so they do not have a position in the world (no transform).
	The tag they use is the tag of the object they are contained within.
	Rendering them as a standalone will result in them always being at the origin.

*******************************************************************************************************************/
#include <string>
#include "managers/FileManager.h"

class Primitive {

	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(COG_NVP(m_tag));
	}

public:
	Primitive(const std::string& tag);
	Primitive() {}
	virtual ~Primitive();

public:
	virtual void Render() = 0;

public:
	void SetTag(const std::string& tag) { m_tag = tag; }

protected:
	std::string m_tag;
};
