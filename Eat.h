#pragma once
#include "CubeSprite.h"
#include <time.h>

class Eat :
	public CubeSprite
{
public:
	Eat(void);
	~Eat(void);
	void setPos();
	void setPos(Vector2f pos);
};
