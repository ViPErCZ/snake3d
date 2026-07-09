#include <snake3d/Renderer/Opengl/RefractionRenderer.h>
#include <snake3d/Manager/RenderManager.h>
#include <GL/glew.h>
#include <iostream>

using namespace std;
using namespace Manager;
using namespace Tools;

namespace Renderer {
    RefractionRenderer::RefractionRenderer(
            const shared_ptr<ContextState> &contextState,
            const shared_ptr<ResourceManager> &resManager,
            const shared_ptr<Camera> &camera,
            const glm::mat4 &projection,
            const int width, const int height)
            : contextState(contextState), resourceManager(resManager), camera(camera),
              projection(projection), width(width), height(height) {
        initializeFramebuffer();
    }

    void RefractionRenderer::initializeFramebuffer() {
        destroyFramebuffer();

        glGenFramebuffers(1, &refractionFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);

        glGenTextures(1, &refractionTexture);
        glBindTexture(GL_TEXTURE_2D, refractionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboW(), fboH(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboW(), fboH());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Refraction Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (resourceManager->hasTexture("RefractionTexture")) {
            resourceManager->getTexture("RefractionTexture")->replaceTexture(refractionTexture);
        } else {
            const auto textureRes = make_shared<TextureManager>(refractionTexture);
            resourceManager->addTexture("RefractionTexture", textureRes);
        }
    }

    void RefractionRenderer::destroyFramebuffer() {
        if (refractionFBO != 0) { glDeleteFramebuffers(1, &refractionFBO); refractionFBO = 0; }
        if (refractionTexture != 0) { glDeleteTextures(1, &refractionTexture); refractionTexture = 0; }
        if (depthBuffer != 0) { glDeleteRenderbuffers(1, &depthBuffer); depthBuffer = 0; }
    }

    RefractionRenderer::~RefractionRenderer() { destroyFramebuffer(); }

    void RefractionRenderer::updateRenderers(const vector<RendererEntry> &renderers) {
        this->renderers = renderers;
    }

    void RefractionRenderer::render3D(const float dt, const uint64_t frameId) {
        // Real (non-mirrored) camera. setReflectionPass(true) forces getViewMatrix() to use
        // the stored position/front (no follow-mode reset) - we just don't mirror anything,
        // so it yields the true main view.
        camera->setReflectionPass(true);

        if (renderManager) {
            Manager::FrameData fd = renderManager->getFrameData();
            fd.view = camera->getViewMatrix();
            fd.viewPos = camera->getPosition();
            renderManager->getFrameUbo().upload(fd);
            renderManager->getFrameUbo().bind();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
        glViewport(0, 0, fboW(), fboH());
        glClearColor(0.04f, 0.16f, 0.28f, 1.0f); // deep-water tone where nothing is drawn
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // `refraction` MODE: the scene renderers skip ONLY the water surface
        // (disableRefraction); the terrain/bed + rocks/trees ARE rendered (that's the
        // bottom we want to see through the water). No winding flip - camera not mirrored.
        for (auto &entry: renderers) {
            if (entry.renderer.get() == this) continue;
            entry.renderer->beforeRender(refraction);
            entry.renderer->render3D(dt, frameId);
            entry.renderer->afterRender();
        }

        contextState->setDepthTest(true);
        contextState->setDepthWrite(true);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        camera->setReflectionPass(false);
    }

    void RefractionRenderer::beforeRender(const MODE mode) { this->mode = mode; }
    void RefractionRenderer::afterRender() {}
    void RefractionRenderer::renderShadowMap() {}

    void RefractionRenderer::resize(const int width, const int height, const glm::mat4 &projection) {
        if (width <= 0 || height <= 0) return;
        this->width = width;
        this->height = height;
        this->projection = projection;
        initializeFramebuffer();
    }

    void RefractionRenderer::setResolutionScale(const float scale) {
        fboScale = std::clamp(scale, 0.1f, 1.0f);
        initializeFramebuffer();
    }
}
