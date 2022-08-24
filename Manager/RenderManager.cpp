#include "RenderManager.h"

namespace Manager {
    RenderManager::RenderManager(int width, int height) : width(width), height(height) {
//        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0f);                            // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
//        glDepthFunc(GL_LESS);
//        glDepthFunc(GL_LEQUAL);                                // The Type Of Depth Testing To Do
//        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations
        glEnable(GL_TEXTURE_2D);
    }

    RenderManager::~RenderManager() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            delete (*Iter);
        }
    }

    void RenderManager::addRenderer(BaseRenderer *renderer) {
        renderers.push_back(renderer);
    }

    void RenderManager::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
//        glClear(GL_COLOR_BUFFER_BIT);
//        setupProjection();
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//
//
//        // zaklad pro animaci pro start hry
//        GLfloat LightAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};// Okolní světlo
//        glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
//        glEnable(GL_LIGHT1);

//        glClearStencil(0);
        glClearColor(.0, .0, .0, 0);


        // TODO: toto pouzit pred startem hry, pro mlhu
        /*
        glEnable(GL_FOG);
        GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };// Tři typy mlhy
        GLuint fogFilter = 0;// Která mlha se používá
        GLfloat fogColor[4] = {0.1f, 0.1f, 0.5f, 1.0f};// Barva mlhy
        glFogi(GL_FOG_MODE, fogMode[fogFilter]);// Mód mlhy
        glFogfv(GL_FOG_COLOR, fogColor);// Barva mlhy
        glFogf(GL_FOG_DENSITY, 0.6f);// Hustota mlhy
        glHint(GL_FOG_HINT, GL_DONT_CARE);// Kvalita mlhy
        glFogf(GL_FOG_START, 1.0f);// Začátek mlhy - v hloubce (osa z)
        glFogf(GL_FOG_END, 100.0f);// Konec mlhy - v hloubce (osa z)
        glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);
         */

        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
//            if ((*Iter)->isOrtho()) {
//                switchOrthoProjection();
//            }
            (*Iter)->beforeRender();
            (*Iter)->render();
            (*Iter)->afterRender();
//            if ((*Iter)->isOrtho()) {
//                restoreProjection();
//            }
        }

//        glDisable(GL_FOG);
    }

    void RenderManager::setWidth(int width) {
        RenderManager::width = width;
    }

    void RenderManager::setHeight(int height) {
        RenderManager::height = height;
    }

    void RenderManager::switchOrthoProjection() const {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0F, width, height, 0.0F, 0.0F, 1.0F);
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        glLoadIdentity();
    }

    void RenderManager::restoreProjection() {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
} // Manager