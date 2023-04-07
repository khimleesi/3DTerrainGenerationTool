#pragma once
/*******************************************************************************************************************
	Background.h, Background.cpp
	Created by Kim Kane
	Last updated: 08/04/2018

	A derived class of base class Interface. Creates a 2D background image.
	
	[Features]

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing.

*******************************************************************************************************************/
#include "Interface.h"
#include "Sprite.h"

class Background : public Interface {
	
	friend class cereal::access;

	/*! @brief Function that contains serialization data for loading and saving. */
	template <class Archive>
	void Serialize(Archive& archive)
	{
		archive(cereal::virtual_base_class<Interface>(this), COG_NVP(m_sprite));
	}

public:
	Background();
	virtual ~Background();

public:
	virtual void Render(Shader* shader) override;
	virtual void Update()				override;

public:
	bool Create(const std::string& tag, const Transform& transform, const std::string& texture);
	bool Load(const std::string& tag);

public:
	Sprite* GetSprite();

private:
	Sprite m_sprite;
};