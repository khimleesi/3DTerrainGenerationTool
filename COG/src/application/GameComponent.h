#pragma once
/*******************************************************************************************************************
	GameComponent.h, GameComponent.cpp
	Created by Kim Kane
	Last updated: 19/05/2018

	A class used for all components in the game. A component could be anything from a renderer to a camera.

	[Features]
	Nothing at present.

	[Upcoming]
	Nothing at present.

	[Side Notes]
	I like the idea of breaking up game objects and game components, but I need to spend more time thinking
	about how I'm going to do this in a nice way, so not much going on here at the mo!

*******************************************************************************************************************/
#include <string>
#include "GameObject.h"

class GameComponent {

public:
	GameComponent(const std::string& tag);
	virtual ~GameComponent();

public:
	virtual void Update() = 0;
	virtual void SetParent(GameObject* parent);

public:
	bool HasParent();
	Transform* GetParentTransform();

private:
	GameComponent(const GameComponent& other) = delete;
	void operator=(const GameComponent& other) = delete;

private:
	std::string m_tag;
	GameObject* m_parent;
};