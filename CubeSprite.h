#pragma once
#include "Sprite.h"

class CubeSprite : public Sprite
{
public:
	CubeSprite();
	~CubeSprite() override;
	void Render() override;
	void setW(float w);
protected:
    float w = 1.0f;
};
