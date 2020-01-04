#pragma once
#include "Sprite.h"
#include <GL/glu.h>
#include <GL/glut.h>
#include "include/fString.h"
#include <SDL2/SDL_ttf.h>
#include <string>

using namespace fTools;

struct sCOLOR {
	GLfloat r, g, b;
};

class TextSprite :
	public Sprite
{
public:
	TextSprite();
	explicit TextSprite(string str);
	~TextSprite() override;
	void Render() override;
	void setText(string str);
	void setColor(GLfloat r, GLfloat g, GLfloat b);

protected:
	string text;
	sCOLOR color{};
};
