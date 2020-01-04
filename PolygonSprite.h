#pragma once
#include "Sprite.h"

class PolygonSprite :
	public Sprite
{
public:
	PolygonSprite(void);
	~PolygonSprite(void);
	virtual void Render();
};
