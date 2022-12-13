#include "RenderManager.h"

namespace Manager {
    RenderManager::RenderManager(int width, int height) : width(width), height(height) {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }

    RenderManager::~RenderManager() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            delete (*Iter);
        }
        delete depthMapRenderer;
    }

    void RenderManager::addRenderer(BaseRenderer *renderer) {
        renderers.push_back(renderer);
    }

    void RenderManager::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        glClearColor(.0, .0, .0, 0);

        if (depthMapRenderer) {
            depthMapRenderer->beforeRender();
        }

        glCullFace(GL_FRONT);
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->beforeRender();
            (*Iter)->render();
            (*Iter)->afterRender();
            (*Iter)->setShadow(true);
        }
        glCullFace(GL_BACK); // don't forget to reset original culling face

        if (depthMapRenderer) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // reset viewport
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            depthMapRenderer->render();
            depthMapRenderer->afterRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->beforeRender();
            (*Iter)->render();
            (*Iter)->afterRender();
            (*Iter)->setShadow(false);
        }
    }

    void RenderManager::setWidth(int width) {
        RenderManager::width = width;
    }

    void RenderManager::setHeight(int height) {
        RenderManager::height = height;
    }

    void RenderManager::setDepthMapRenderer(DepthMapRenderer *depthMapRenderer) {
        RenderManager::depthMapRenderer = depthMapRenderer;
    }

} // Manager