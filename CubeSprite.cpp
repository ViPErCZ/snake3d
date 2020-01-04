#include "stdafx.h"
#include "CubeSprite.h"

CubeSprite::CubeSprite()
= default;

CubeSprite::~CubeSprite()
= default;

void CubeSprite::Render() {
    if (this->getVisibility()) {
        Sprite::Render();

        glBegin(GL_QUADS);

        // P�edn� st�na
        glNormal3f(0.0f, 0.0f, 1.0f);// Norm�la
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, 1.0f, this->w);

        // Zadn� st�na
        glNormal3f(0.0f, 0.0f, -1.0f);// Norm�la
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);

        // Horn� st�na
        glNormal3f(0.0f, 1.0f, 0.0f);// Norm�la
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(-1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);

        // Spodn� st�na
        glNormal3f(0.0f, -1.0f, 0.0f);// Norm�la
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(-1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, 1.0f, this->w);

        // Prav� st�na
        glNormal3f(1.0f, 0.0f, 0.0f);// Norm�la
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);

        // Lev� st�na
        glNormal3f(-1.0f, 0.0f, 0.0f);// Norm�la
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, -1.0f, this->w);

        glEnd();
    }
}

void CubeSprite::setW(float w) {
    this->w = w;
}
