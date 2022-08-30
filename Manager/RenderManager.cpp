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
        glClearColor(.0, .0, .0, 0);

        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->beforeRender();
            (*Iter)->render();
            (*Iter)->afterRender();
        }
    }

    void RenderManager::setWidth(int width) {
        RenderManager::width = width;
    }

    void RenderManager::setHeight(int height) {
        RenderManager::height = height;
    }
} // Manager