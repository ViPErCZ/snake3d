#include "RadarRenderer.h"

namespace Renderer {
    RadarRenderer::RadarRenderer(Radar* radar): radar(radar) {
        ortho = true;
    }

    RadarRenderer::~RadarRenderer() {
        delete radar;
    }

    void RadarRenderer::render() {
        if (radar->isVisible()) {
            float LightPos[4]={5.0f,5.0f,00.0f,1.0f};
            float Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
            glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);

            GLuint index = radar->getPrimaryTexture();
            if (index > 0) {
                glBindTexture(GL_TEXTURE_2D, index);
            }
            glLoadIdentity();

            glm::vec3 position = radar->getPosition();
            glm::vec3 zoom = radar->getZoom();
            const glm::vec4 * rotate = radar->getRotate();
            glTranslatef( position.x, position.y, position.z );
            glScalef(zoom.x, zoom.y, zoom.z);
            glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
            glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
            glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);


            glBegin(GL_POLYGON);
            glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0.0f, 0.0f );
            glTexCoord2f( 1.0f, 0.0f ); glVertex2f( radar->getWidth(), 0.0f );
            glTexCoord2f( 1.0f, 1.0f ); glVertex2f( radar->getWidth(), radar->getHeight() );
            glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0.0f, radar->getHeight() );
            glEnd( );

            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            radar->updatePositions();

            for(auto Iter = radar->getItems().end() - 1; Iter >= radar->getItems().begin(); Iter--) {
                if (!(*Iter).item->isVisible()) {
                    continue;
                }
                GLuint index = (*Iter).radarPresent->getPrimaryTexture();
                if (index > 0) {
                    glBindTexture(GL_TEXTURE_2D, index);
                }

                glLoadIdentity( );
                glm::vec3 position = (*Iter).radarPresent->getPosition();
                glm::vec3 zoom = (*Iter).radarPresent->getZoom();
                const glm::vec4 * rotate = (*Iter).radarPresent->getRotate();
                glTranslatef( position.x, position.y, position.z );
                glScalef(zoom.x, zoom.y, zoom.z);
                glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
                glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
                glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);

                glBegin(GL_POLYGON);
                glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0.0f, 0.0f );
                glTexCoord2f( 1.0f, 0.0f ); glVertex2f( (*Iter).radarPresent->getWidth(), 0.0f );
                glTexCoord2f( 1.0f, 1.0f ); glVertex2f( (*Iter).radarPresent->getWidth(), (*Iter).radarPresent->getHeight() );
                glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0.0f, (*Iter).radarPresent->getHeight() );
                glEnd( );
            }

            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
        }
    }

    void RadarRenderer::beforeRender() {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_BLEND);
        glColor4f(0.3f, 0.0f, 0.0f, 0.0f);
    }

    void RadarRenderer::afterRender() {
        glDisable(GL_BLEND);

        glPopAttrib();
        glPopMatrix();
    }
} // Renderer