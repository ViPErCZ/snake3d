#include "RenderManager.h"

namespace Manager {
    RenderManager::RenderManager(const shared_ptr<Camera> &camera, const shared_ptr<ResourceManager> &resourceManager,
                                 const glm::mat4 &projection, const int width, const int height) : camera(camera),
        resourceManager(resourceManager), width(width), projection(projection), height(height), shadows(false),
        bloom(false), fog(false) {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }

    void RenderManager::initBloom() {
        bloomRenderer = make_unique<BloomRenderer>(resourceManager, width, height);
    }

    void RenderManager::initShadowMapping() {
        depthMapRenderer = make_unique<DepthMapRenderer>(camera.get(), projection, resourceManager.get());
    }

    void RenderManager::addRenderer(shared_ptr<BaseRenderer> renderer, const int priority) {
        renderers.push_back({std::move(renderer), priority});
        stable_sort(renderers.begin(), renderers.end(),
                     [](auto &a, auto &b) { return a.priority > b.priority; });
    }

    void RenderManager::render(const float dt) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glEnable(GL_DEPTH_TEST);
        glLoadIdentity();
        if (!bloom) {
            glClearColor(.0, .0, .0, 1.0);
        } else {
            constexpr auto backgroundColor = glm::vec3(0.0, 0.0, 0.0);
            constexpr float backgroundIntensity = {2.0f};
            glClearColor(
                backgroundColor.r * backgroundIntensity,
                backgroundColor.g * backgroundIntensity,
                backgroundColor.b * backgroundIntensity,
                1.0f
            );
        }

        glViewport(0, 0, width, height);

        if (shadows && depthMapRenderer) {
            glDepthFunc(GL_LESS);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(3.0f, 3.0f);

            glm::vec3 sceneMin(FLT_MAX);
            glm::vec3 sceneMax(-FLT_MAX);

            for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                sceneMin = Iter->renderer->compareSceneMin(sceneMin);
                sceneMax = Iter->renderer->compareSceneMax(sceneMax);
            }

            constexpr float padding = 2.0f;
            sceneMin -= glm::vec3(padding);
            sceneMax += glm::vec3(padding);

            constexpr glm::vec3 centerScene = {0, 0, 0}; //(sceneMin + sceneMax) / 2.0f;

            // TODO: dirLight dodelat object a dosadit do render manageru
            auto light = make_shared<DirectionalLight>(DirectionalLight());
            light->setPosition({0.0f, 7.0f, 11.0f});
            light->setDirection({1, 1.0, -3});
            //const auto lightSpacesMatrix = depthMapRenderer->computeLightSpaceMatrixForPlane(light, centerScene, 14, 14);
            const auto lightSpacesMatrix = depthMapRenderer->computeLightSpaceMatrix(light, centerScene, sceneMin, sceneMax);
            int index = 0;

            for (auto & matrix : lightSpacesMatrix) {
                depthMapRenderer->beforeRender(index);
                depthMapRenderer->bind(index, matrix);

                glCullFace(GL_FRONT);

                for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                    if (Iter->renderer->isShadow()) {
                        Iter->renderer->renderShadowMap();
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

        if (bloom) {
            bloomRenderer->beforeRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->beforeRender();
            Iter->renderer->render3D(dt);
            Iter->renderer->afterRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->beforeRender();
            Iter->renderer->render2D(dt);
            Iter->renderer->afterRender();
        }

        if (bloom) {
            this->bloomRenderer->afterRender();
        }
    }

    void RenderManager::setWidth(const int width) {
        RenderManager::width = width;
    }

    void RenderManager::setHeight(const int height) {
        RenderManager::height = height;
    }

    void RenderManager::setDepthMapRenderer(unique_ptr<DepthMapRenderer> &depthMapRenderer) {
        RenderManager::depthMapRenderer = std::move(depthMapRenderer);
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
            Iter->renderer->setShadow(shadows);
        }
    }

    void RenderManager::toggleShadows() {
        shadows = !shadows;
        updateShadows();
    }

    void RenderManager::setBloomRenderer(unique_ptr<BloomRenderer> &bloomRenderer) {
        RenderManager::bloomRenderer = std::move(bloomRenderer);
    }

    void RenderManager::toggleBloom() {
        bloom = !bloom;
    }

    void RenderManager::toggleFog() {
        fog = !fog;
        updateFog();
    }

    void RenderManager::reset() {
        renderers.clear();
    }

    void RenderManager::updateFog() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->setFog(fog);
        }
    }

}