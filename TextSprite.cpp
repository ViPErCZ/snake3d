#include <utility>
#include "stdafx.h"
#include "TextSprite.h"

TextSprite::TextSprite()
{
	color.r = color.g = color.b = 1;
}

TextSprite::TextSprite(string str)
{
	text = std::move(str);
	color.r = color.g = color.b = 0.2;
}

TextSprite::~TextSprite(void)
= default;

void TextSprite::Render(int width, int height) {
    if (isVisible) {
        Sprite::Render();

        // switch to projection mode
        glMatrixMode(GL_PROJECTION);
        // save previous matrix which contains the
        //settings for the perspective projection
        glPushMatrix();

        // reset matrix
        glLoadIdentity();

        // set a 2D orthographic projection
        glOrtho(0.0F, width, height, 0.0F, 0.0F, 1.0F);

        // switch back to modelview mode
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        glLoadIdentity();

        // ------------------------------------------------------
        glDisable(GL_TEXTURE_2D);
        float LightPos[4] = {-5.0f, 5.0f, 10.0f, 1.0f};
        float Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float Ambient2[4] = {0.0f, 0.2f, 0.9f, 10.5f};;
        glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
        glColor3f(color.r, color.g, color.b);
        glRasterPos2i((GLint) this->getPos().x, (GLint) this->getPos().y);
        //glRasterPos2i(3, 15);
        const char *str = text.c_str();
        while (*str) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str++);
        }

        glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient2);

        glPopAttrib();
        glPopMatrix();

        // restore
        // --------------------------------
        glMatrixMode(GL_PROJECTION);
        // restore previous projection matrix
        glPopMatrix();

        // get back to modelview mode
        glMatrixMode(GL_MODELVIEW);
    }
}

void TextSprite::setText(string str) {
	text = std::move(str);
}

void TextSprite::setColor(GLfloat r, GLfloat g, GLfloat b) {
	color.r = r;
	color.g = g;
	color.b = b;
}