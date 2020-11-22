#pragma once
#include "CubeSprite.h"
#include <ctime>

class Eat :
	public CubeSprite
{
public:
	Eat();
	~Eat() override;
	void setPos();
	void setPos(Vector2f pos);
    void Render() override;

protected:
};
