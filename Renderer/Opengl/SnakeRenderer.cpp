#include "SnakeRenderer.h"

namespace Renderer {
    SnakeRenderer::SnakeRenderer(Snake *item) {
        snake = item;
    }

    SnakeRenderer::~SnakeRenderer() {
        delete snake;
    }

    void SnakeRenderer::render() {
        for (auto Iter = snake->getItems().begin(); Iter < snake->getItems().end(); Iter++) {
            if ((*Iter)->tile->isVisible()) {

                if (Iter == snake->getItems().begin()) {
                    glBindTexture(GL_TEXTURE_2D, 6);
                } else {
                    glBindTexture(GL_TEXTURE_2D, 7);
                }

                glLoadIdentity( );
                glm::vec3 pos = (*Iter)->tile->getPosition();
                glm::vec3 zoom = (*Iter)->tile->getZoom();
                const glm::vec4 * rotate = (*Iter)->tile->getRotate();

                glTranslatef( pos.x, pos.y, pos.z );
                glScalef(zoom.x, zoom.y, zoom.z);
                glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
                glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
                glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);

                glBegin(GL_QUADS);

                // Přední stěna
                glNormal3f(0.0f, 0.0f, 1.0f);// Normála
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);

                // Zadní stěna
                glNormal3f(0.0f, 0.0f, -1.0f);// Normála
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);

                // Horní stěna
                glNormal3f(0.0f, 1.0f, 0.0f);// Normála
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);

                // Spodní stěna
                glNormal3f(0.0f, -1.0f, 0.0f);// Normála
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);

                // Pravá stěna
                glNormal3f(1.0f, 0.0f, 0.0f);// Normála
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(1.0f, -1.0f, 1.0f, 1.0f);

                // Levá stěna
                glNormal3f(-1.0f, 0.0f, 0.0f);// Normála
                glTexCoord2f(0.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, -1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex4f(-1.0f, -1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, 1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex4f(-1.0f, 1.0f, -1.0f, 1.0f);

                glEnd();
            }
        }
    }

    void SnakeRenderer::beforeRender() {
        float LightPos[4]={60.0f,20.0f,10.0f,5.0f};
        float Ambient[4]={0.0f,0.2f,0.9f,10.5f};

        glEnable(GL_FOG);
        glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
        glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
    }

    void SnakeRenderer::afterRender() {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
    }
} // Renderer