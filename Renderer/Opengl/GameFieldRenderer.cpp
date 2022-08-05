#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item) {
    gameField = item;
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
}

void Renderer::GameFieldRenderer::render() {
    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        if ((*Iter)->isVisible()) {
            GLuint index = (*Iter)->findTexture(5);
            if (index > 0) {
                glBindTexture(GL_TEXTURE_2D, index);
            }
            glLoadIdentity();

            glm::vec3 position = (*Iter)->getPosition();
            glm::vec3 zoom = (*Iter)->getZoom();
            const glm::vec4 * rotate = (*Iter)->getRotate();
            glTranslatef( position.x, position.y, position.z );
            glScalef(zoom.x, zoom.y, zoom.z);
            glRotatef(rotate[0].x, rotate[0].y, rotate[0].z, rotate[0].w);
            glRotatef(rotate[1].x, rotate[1].y, rotate[1].z, rotate[1].w);
            glRotatef(rotate[2].x, rotate[2].y, rotate[2].z, rotate[2].w);

            glBegin(GL_POLYGON);
            glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0.0f, 0.0f );
            glTexCoord2f( 1.0f, 0.0f ); glVertex2f( (*Iter)->getWidth(), 0.0f );
            glTexCoord2f( 1.0f, 1.0f ); glVertex2f( (*Iter)->getWidth(), (*Iter)->getHeight() );
            glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0.0f, (*Iter)->getHeight() );
            glEnd( );
        }
    }
}

void Renderer::GameFieldRenderer::beforeRender() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float LightPos[4]={5.0f,5.0f,00.0f,1.0f};
        float Ambient[4]={0.0f,0.2f,0.9f,1.0f};
        glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
        glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);

        GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };// Tři typy mlhy
        GLuint fogFilter = 2;// Která mlha se používá
        GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};// Barva mlhy
        glFogi(GL_FOG_MODE, fogMode[fogFilter]);// Mód mlhy
        glFogfv(GL_FOG_COLOR, fogColor);// Barva mlhy
        glFogf(GL_FOG_DENSITY, 0.05f);// Hustota mlhy
        glHint(GL_FOG_HINT, GL_DONT_CARE);// Kvalita mlhy
        glFogf(GL_FOG_START, 1.0f);// Začátek mlhy - v hloubce (osa z)
        glFogf(GL_FOG_END, 120.0f);// Konec mlhy - v hloubce (osa z)
        //glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);
        glEnable(GL_FOG);// Zapne mlhu

        float LightPos2[4]={0.5f, 1.0f, 0.3f,0.0f};
        float Ambient2[4]={1.0f,0.0f,0.0f,1.0f};
        glLightfv(GL_LIGHT0,GL_POSITION,LightPos2);
        glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient2);
}

void Renderer::GameFieldRenderer::afterRender() {
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}
