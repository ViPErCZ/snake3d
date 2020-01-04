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

        // Pøední stìna
        glNormal3f(0.0f, 0.0f, 1.0f);// Normála
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, 1.0f, this->w);

        // Zadní stìna
        glNormal3f(0.0f, 0.0f, -1.0f);// Normála
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);

        // Horní stìna
        glNormal3f(0.0f, 1.0f, 0.0f);// Normála
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(-1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(-1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);

        // Spodní stìna
        glNormal3f(0.0f, -1.0f, 0.0f);// Normála
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(-1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(-1.0f, -1.0f, 1.0f, this->w);

        // Pravá stìna
        glNormal3f(1.0f, 0.0f, 0.0f);// Normála
        glTexCoord2f(1.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, -1.0f, this->w);
        glTexCoord2f(1.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, -1.0f, this->w);
        glTexCoord2f(0.0f, 1.0f);
        glVertex4f(1.0f, 1.0f, 1.0f, this->w);
        glTexCoord2f(0.0f, 0.0f);
        glVertex4f(1.0f, -1.0f, 1.0f, this->w);

        // Levá stìna
        glNormal3f(-1.0f, 0.0f, 0.0f);// Normála
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
