#pragma once
/*******************************************************************************************************************
	MinimapWidget.h, MinimapWidget.cpp
	Created by Kim Kane
	Last updated: 29/04/2018

	Derives from Widget. Creates an interface widget to surround the minimap.

	[Features]
	Nothing fancy.

	[Upcoming]
	Nothing.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include "Minimap.h"
#include "Widget.h"

class MinimapWidget : public Widget {

public:
	MinimapWidget(const std::string& tag, const std::string& texture, const Transform& transform);
	~MinimapWidget();

public:
	virtual void Update()				override;
	virtual void Render(Shader* shader) override;

public:
	static MinimapWidget* Create(const std::string& tag);

public:
	Minimap* GetMinimap();

private:
	Minimap m_minimap;
};