#include "PlanarReflectionRenderer.h"
#include "../../Manager/RenderManager.h"
#include <GL/glew.h>
#include <iostream>
#include "Model/Standard/PlaneMesh.h"

namespace Renderer {
    PlanarReflectionRenderer::PlanarReflectionRenderer(
            const shared_ptr<ContextState> &contextState,
            const shared_ptr<ResourceManager> &resManager,
            const shared_ptr<Camera> &camera,
            const glm::mat4 &projection,
            const int width, const int height)
            : contextState(contextState), resourceManager(resManager), camera(camera), projection(projection), width(width), height(height) {
        initializeFramebuffer();
    }

    void PlanarReflectionRenderer::initializeFramebuffer() {
        destroyFramebuffer();

        glGenFramebuffers(1, &reflectionFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);

        glGenTextures(1, &reflectionTexture);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Reflection Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (resourceManager->hasTexture("PlanarReflectionTexture")) {
            resourceManager->getTexture("PlanarReflectionTexture")->replaceTexture(reflectionTexture);
        } else {
            const auto textureRes = make_shared<TextureManager>(reflectionTexture);
            resourceManager->addTexture("PlanarReflectionTexture", textureRes);
        }
    }

    void PlanarReflectionRenderer::destroyFramebuffer() {
        if (reflectionFBO != 0) {
            glDeleteFramebuffers(1, &reflectionFBO);
            reflectionFBO = 0;
        }
        if (reflectionTexture != 0) {
            glDeleteTextures(1, &reflectionTexture);
            reflectionTexture = 0;
        }
        if (depthBuffer != 0) {
            glDeleteRenderbuffers(1, &depthBuffer);
            depthBuffer = 0;
        }
    }

    PlanarReflectionRenderer::~PlanarReflectionRenderer() {
        destroyFramebuffer();
    }

    void PlanarReflectionRenderer::updateRenderers(const vector<RendererEntry> &renderers) {
        this->renderers = renderers;
    }

    void PlanarReflectionRenderer::render3D(const float dt, const uint64_t frameId) {
        // Reflect the camera across the Z plane so sub-renderers see the mirrored scene.
        // setReflectionPass(true) forces getViewMatrix() to use the stored position/front
        // instead of the follow-mode path, which would otherwise reset them.
        glm::vec3 originalPos = camera->getPosition();
        const glm::vec3 originalFront = camera->getFront();
        const glm::vec3 originalUp = camera->getUp();

        const float dist = 2.0f * (originalPos.z - planeZ);
        camera->setPosition({originalPos.x, originalPos.y, originalPos.z - dist});

        glm::vec3 reflectedFront = originalFront;
        reflectedFront.z = -reflectedFront.z;
        camera->setFront(reflectedFront);

        glm::vec3 reflectedUp = originalUp;
        reflectedUp.z = -reflectedUp.z;
        camera->setUp(reflectedUp);

        camera->setReflectionPass(true);

        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glFrontFace(GL_CW);

        for (auto &entry: renderers) {
            if (entry.renderer.get() == this) continue;

            entry.renderer->beforeRender(reflection);
            entry.renderer->render3D(dt, frameId);
            entry.renderer->afterRender();
        }

        contextState->setDepthTest(true);
        contextState->setDepthWrite(true);
        glFrontFace(GL_CCW);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        camera->setReflectionPass(false);
        camera->setPosition(originalPos);
        camera->setFront(originalFront);
        camera->setUp(originalUp);
    }

    void PlanarReflectionRenderer::beforeRender(const MODE mode) {
        this->mode = mode;
    }

    void PlanarReflectionRenderer::afterRender() {
    }

    void PlanarReflectionRenderer::renderShadowMap() {
    }

    void PlanarReflectionRenderer::setPlaneZ(const float z) {
        planeZ = z;
    }

    void PlanarReflectionRenderer::resize(const int width, const int height, const glm::mat4 &projection) {
        if (width <= 0 || height <= 0) {
            return;
        }
        this->width = width;
        this->height = height;
        this->projection = projection;
        initializeFramebuffer();
    }
}
