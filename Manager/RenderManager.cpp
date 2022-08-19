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

    void RenderManager::setupProjection() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(40, (GLfloat)width/(GLfloat)height, 1.5f, 2600.0f);
//        glLoadMatrixf(glm::value_ptr(perspective));
//        glTranslatef(0, 0, -1050); // correctly center
        glTranslatef(54, -247, -450); // correctly center
        glRotatef(-60, 1, 0, 0);
        glRotatef(41, 0, 0, 1);

        float x = stickyPoint->getPosition().x - 154; // head x
        float y = stickyPoint->getPosition().y - 154; // head y
        int down = -147;

        // camera
        glm::vec3 cameraPos = glm::vec3(x, y + 0.0025 * std::abs(sin(1 * 3.14 / 180)) + down, 1.0f);
        glm::vec3 cameraFront = glm::vec3(x, y + 0.0025 * std::abs(sin(1 * 3.14 / 180)) + down, -1.0f);

        /** upside down */
//        glRotatef(180, 0, 1, 0);
//        glRotatef(60, 1, 0, 0);

        gluLookAt(cameraPos.x,
                  cameraPos.y,
                  0.0,
                  cameraFront.x,
                  cameraPos.y,
                  -1.0,
                  0.0,
                  1.0,
                  0.0);
    }

    void RenderManager::setStickyPoint(BaseItem *stickyPoint) {
        this->stickyPoint = stickyPoint;
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