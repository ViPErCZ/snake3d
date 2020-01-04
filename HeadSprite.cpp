//
// Created by viper on 02.01.20.
//

#include "HeadSprite.h"

void HeadSprite::Render() {
    Sprite::Render();

    glBegin(GL_QUADS);

    // Přední stěna
    glNormal3f( 0.0f, 0.0f, 1.0f);// Normála
    glTexCoord2f(0.0f, 0.0f); glVertex4f(-1.0f, -1.0f, 1.005f, this->w);
    glTexCoord2f(1.0f, 0.0f); glVertex4f( 1.0f, -1.0f, 1.005f, this->w);
    glTexCoord2f(1.0f, 1.0f); glVertex4f( 1.0f, 1.0f, 1.005f, this->w);
    glTexCoord2f(0.0f, 1.0f); glVertex4f(-1.0f, 1.0f, 1.005f, this->w);

    // Zadní stěna
    glNormal3f( 0.0f, 0.0f,-1.0f);// Normála
    glTexCoord2f(1.0f, 0.0f); glVertex4f(-1.0f, -1.0f, -1.005f, this->w);
    glTexCoord2f(1.0f, 1.0f); glVertex4f(-1.0f, 1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 1.0f); glVertex4f( 1.0f, 1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 0.0f); glVertex4f( 1.0f, -1.0f, -1.005f, this->w);

    // Horní stěna
    glNormal3f( 0.0f, 1.0f, 0.0f);// Normála
    glTexCoord2f(0.0f, 1.0f); glVertex4f(-1.0f, 1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 0.0f); glVertex4f(-1.0f, 1.0f, 1.005, this->w);
    glTexCoord2f(1.0f, 0.0f); glVertex4f( 1.0f, 1.0f, 1.005f, this->w);
    glTexCoord2f(1.0f, 1.0f); glVertex4f( 1.0f, 1.0f, -1.005f, this->w);

    // Spodní stěna
    glNormal3f( 0.0f,-1.0f, 0.0f);// Normála
    glTexCoord2f(1.0f, 1.0f); glVertex4f(-1.0f, -1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 1.0f); glVertex4f( 1.0f, -1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 0.0f); glVertex4f( 1.0f, -1.0f, 1.005f, this->w);
    glTexCoord2f(1.0f, 0.0f); glVertex4f(-1.0f, -1.0f, 1.005f, this->w);

    // Pravá stěna
    glNormal3f( 1.0f, 0.0f, 0.0f);// Normála
    glTexCoord2f(1.0f, 0.0f); glVertex4f( 1.0f, -1.0f, -1.005f, this->w);
    glTexCoord2f(1.0f, 1.0f); glVertex4f( 1.0f, 1.0f, -1.005f, this->w);
    glTexCoord2f(0.0f, 1.0f); glVertex4f( 1.0f, 1.0f, 1.005f, this->w);
    glTexCoord2f(0.0f, 0.0f); glVertex4f( 1.0f, -1.0f, 1.005f, this->w);

    // Levá stěna
    glNormal3f(-1.0f, 0.0f, 0.0f);// Normála
    glTexCoord2f(0.0f, 0.0f); glVertex4f(-1.0f, -1.0f, -1.001f, this->w);
    glTexCoord2f(1.0f, 0.0f); glVertex4f(-1.0f, -1.0f, 1.001f, this->w);
    glTexCoord2f(1.0f, 1.0f); glVertex4f(-1.0f, 1.0f, 1.001f, this->w);
    glTexCoord2f(0.0f, 1.0f); glVertex4f(-1.0f, 1.0f, -1.001f, this->w);

    glEnd();
}