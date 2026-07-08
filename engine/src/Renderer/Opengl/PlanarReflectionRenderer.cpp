#include <snake3d/Renderer/Opengl/PlanarReflectionRenderer.h>
#include <snake3d/Manager/RenderManager.h>
#include <GL/glew.h>
#include <iostream>
#include <snake3d/Renderer/Opengl/Model/Standard/PlaneMesh.h>

using namespace std;
using namespace Manager;
using namespace Tools;
using namespace Lights;

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboW(), fboH(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboW(), fboH());
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

        // Mirror the camera across the plane dot(n, p) = d. For a point:
        //   p' = p - 2*(dot(n,p) - d)*n
        // for a direction (front/up):  v' = v - 2*dot(n,v)*n.
        // With n=+Z, d=planeZ this is exactly the legacy Z-flip (snake3 unchanged).
        const glm::vec3 n = reflectNormal;
        const float d = reflectOffset;
        camera->setPosition(originalPos - 2.0f * (glm::dot(n, originalPos) - d) * n);
        camera->setFront(originalFront - 2.0f * glm::dot(n, originalFront) * n);
        camera->setUp(originalUp - 2.0f * glm::dot(n, originalUp) * n);

        camera->setReflectionPass(true);

        // Capture the mirrored camera's view-projection for world-anchored reflection
        // sampling (0 A.D.'s reflectionMatrix).
        mirrorViewProj = projection * camera->getViewMatrix();

        if (renderManager) {
            // Copy current main-pass frame data (dirLight, lights, time) and
            // override only the camera-derived fields for the mirror view.
            Manager::FrameData mirror = renderManager->getFrameData();
            mirror.view = camera->getViewMatrix();
            mirror.viewPos = camera->getPosition();
            renderManager->getFrameUbo().upload(mirror);
            renderManager->getFrameUbo().bind();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glViewport(0, 0, fboW(), fboH());
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
        reflectNormal = glm::vec3(0.0f, 0.0f, 1.0f);
        reflectOffset = z;
    }

    void PlanarReflectionRenderer::setReflectionPlane(const glm::vec3 &normal, const float offset) {
        const float len = glm::length(normal);
        reflectNormal = (len > 1e-6f) ? normal / len : glm::vec3(0.0f, 0.0f, 1.0f);
        reflectOffset = offset;
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

    void PlanarReflectionRenderer::setResolutionScale(const float scale) {
        fboScale = std::clamp(scale, 0.1f, 1.0f);
        initializeFramebuffer();
    }
}
