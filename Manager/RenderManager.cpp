#include "RenderManager.h"

namespace Manager {
    RenderManager::RenderManager(const int width, const int height) :
        width(width), height(height), shadows(false), bloom(false), fog(false) {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }

    RenderManager::~RenderManager() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            delete (*Iter);
        }
        delete depthMapRenderer;
        delete bloomRenderer;
    }

    void RenderManager::addRenderer(BaseRenderer *renderer) {
        renderers.push_back(renderer);
    }

    void RenderManager::render(float dt) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        if (!bloom) {
            glClearColor(.0, .0, .0, 1.0);
        } else {
            glm::vec3 backgroundColor = glm::vec3(0.0, 0.0, 0.0);
            float backgroundIntensity = {2.0f};
            glClearColor(
                backgroundColor.r * backgroundIntensity,
                backgroundColor.g * backgroundIntensity,
                backgroundColor.b * backgroundIntensity,
                1.0f
            );
        }

        glViewport(0, 0, width, height);

        if (shadows && depthMapRenderer) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(3.0f, 3.0f);

            glm::vec3 sceneMin(FLT_MAX);
            glm::vec3 sceneMax(-FLT_MAX);

            for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                sceneMin = (*Iter)->compareSceneMin(sceneMin);
                sceneMax = (*Iter)->compareSceneMax(sceneMax);
            }

            // přidej padding
            constexpr float padding = 2.0f;
            sceneMin -= glm::vec3(padding);
            sceneMax += glm::vec3(padding);

            constexpr glm::vec3 centerScene = {0, 0, 0}; //(sceneMin + sceneMax) / 2.0f;

            // TODO: dirLight dodelat object a dosadit do render manageru
            auto light = make_shared<DirectionalLight>(DirectionalLight());
            light->setPosition({0.0f, 7.0f, 11.0f});
            light->setDirection({1, 1.0, -3});
            const auto lightSpacesMatrix = depthMapRenderer->computeLightSpaceMatrix(light, centerScene, sceneMin, sceneMax);
            int index = 0;

            for (auto & matrix : lightSpacesMatrix) {
                depthMapRenderer->beforeRender(index);
                depthMapRenderer->bind(index, matrix);

                glCullFace(GL_FRONT);

                for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                    if ((*Iter)->isShadow()) {
                        (*Iter)->renderShadowMap();
                    }
                }
                glCullFace(GL_BACK);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // reset viewport
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                depthMapRenderer->render(dt);
                depthMapRenderer->afterRender();

                index++;
            }

            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        constexpr GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        glEnable(GL_DEPTH_TEST);

        if (bloom) {
            bloomRenderer->beforeRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            (*Iter)->beforeRender();
            (*Iter)->render(dt);
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
        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
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