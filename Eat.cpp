#include "stdafx.h"
#include "Eat.h"

Eat::Eat() = default;

Eat::~Eat()
= default;

void Eat::setPos() {
	srand(time(nullptr));

	int fieldX = rand() % 46 + 1; // 46 poli

	srand(time(nullptr));

	int fieldY = rand() % 23 + 1;

	Vector3f pos = {static_cast<float>(fieldX*32+16), static_cast<float>(fieldY*32+16), 15};

	CubeSprite::setPos(pos);
}



void Eat::setPos(Vector2f pos) {
	Vector3f posSprite = {pos.x, pos.y + 0, 15};

	CubeSprite::setPos(posSprite);
}

void Eat::Render() {
//    Uint32 now = SDL_GetTicks();

    if (this->getVisibility()) {
        if (!textures.empty()) {
            glBindTexture(GL_TEXTURE_2D, getActiveTexture());
        }

        glLoadIdentity( );

        glTranslatef( pos.x, pos.y, pos.z );
        glScalef(zoom.x, zoom.y, zoom.z);
        glRotatef(rotate[0].angle, rotate[0].x, rotate[0].y, rotate[0].z);
        glRotatef(rotate[1].angle, rotate[1].x, rotate[1].y, rotate[1].z);
        glRotatef(rotate[2].angle, rotate[2].x, rotate[2].y, rotate[2].z);

        const float PI = 3.141592f;
        GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles
        GLfloat radius = 1.0f;
        int gradation = 20;

        for (alpha = 0.0; alpha < PI; alpha += PI/gradation)
        {
            glBegin(GL_TRIANGLE_STRIP);
            for (beta = 0.0; beta < 2.01*PI; beta += PI/gradation)
            {
                x = radius*cos(beta)*sin(alpha);
                y = radius*sin(beta)*sin(alpha);
                z = radius*cos(alpha);
                glVertex3f(x, y, z);
                x = radius*cos(beta)*sin(alpha + PI/gradation);
                y = radius*sin(beta)*sin(alpha + PI/gradation);
                z = radius*cos(alpha + PI/gradation);
                glVertex3f(x, y, z);
            }
            glEnd();
        }
    }
}
