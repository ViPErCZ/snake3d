#include "RenderManager.h"

namespace Manager {
    RenderManager::RenderManager(int width, int height) :
        width(width), height(height), bloom(false), shadows(false), fog(false) {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }

    RenderManager::~RenderManager() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            delete (*Iter);
        }
        delete depthMapRenderer;
        delete bloomRenderer;
    }

    void RenderManager::addRenderer(BaseRenderer *renderer) {
        renderers.push_back(renderer);
    }

    void RenderManager::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        glClearColor(.0, .0, .0, 0);
        glViewport(0, 0, width, height);

        if (shadows) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(3.0f, 3.0f);
            if (depthMapRenderer) {
                depthMapRenderer->beforeRender();
            }

            glCullFace(GL_FRONT);
            for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
                if ((*Iter)->isShadow()) {
                    (*Iter)->renderShadowMap();
                }
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
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if (bloom) {
            bloomRenderer->beforeRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->beforeRender();
            (*Iter)->render();
            (*Iter)->afterRender();
        }

        if (bloom) {
            this->bloomRenderer->afterRender();
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

    void RenderManager::enableShadows() {
        shadows = true;
        updateShadows();
    }

    void RenderManager::disableShadows() {
        shadows = false;
        updateShadows();
    }

    void RenderManager::updateShadows() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->setShadow(shadows);
        }
    }

    void RenderManager::toggleShadows() {
        shadows = !shadows;
        updateShadows();
    }

    void RenderManager::setBloomRenderer(BloomRenderer *bloomRenderer) {
        RenderManager::bloomRenderer = bloomRenderer;
    }

    void RenderManager::toggleBloom() {
        bloom = !bloom;
    }

    void RenderManager::toggleFog() {
        fog = !fog;
        updateFog();
    }

    void RenderManager::updateFog() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); Iter++) {
            (*Iter)->setFog(fog);
        }
    }

}