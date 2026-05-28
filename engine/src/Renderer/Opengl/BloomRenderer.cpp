#include <snake3d/Renderer/Opengl/BloomRenderer.h>
#include <snake3d/Resource/ShaderLoader.h>

using namespace Manager;
using namespace std;

Renderer::BloomRenderer::BloomRenderer(const shared_ptr<ResourceManager> &resManager, const int width, const int height)
    : resourceManager(resManager), width(width), height(height) {

    shaderBlur = resourceManager->getShader("blur");
    shaderBloomFinal = resourceManager->getShader("bloomFinal");
    initializeFramebuffers();

    shaderBlur->use();
    shaderBlur->setInt("image", 0);
    shaderBloomFinal->use();
    shaderBloomFinal->setInt("scene", 0);
    shaderBloomFinal->setInt("bloomBlur", 1);
}

Renderer::BloomRenderer::~BloomRenderer() {
    destroyFramebuffers();
    if (quadVAO != 0) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO != 0) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }
}

void Renderer::BloomRenderer::initializeFramebuffers() {
    destroyFramebuffers();

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    constexpr unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    if (resourceManager->hasTexture("SceneTexture")) {
        resourceManager->getTexture("SceneTexture")->replaceTexture(colorBuffers[0]);
    } else {
        const auto sceneTexture = make_shared<TextureManager>(colorBuffers[0]);
        resourceManager->addTexture("SceneTexture", sceneTexture);
    }
}

void Renderer::BloomRenderer::destroyFramebuffers() {
    if (hdrFBO != 0) {
        glDeleteFramebuffers(1, &hdrFBO);
        hdrFBO = 0;
    }
    glDeleteFramebuffers(2, pingpongFBO);
    pingpongFBO[0] = 0;
    pingpongFBO[1] = 0;
    glDeleteTextures(2, colorBuffers);
    colorBuffers[0] = 0;
    colorBuffers[1] = 0;
    glDeleteTextures(2, pingpongColorBuffers);
    pingpongColorBuffers[0] = 0;
    pingpongColorBuffers[1] = 0;
    if (rboDepth != 0) {
        glDeleteRenderbuffers(1, &rboDepth);
        rboDepth = 0;
    }
}

void Renderer::BloomRenderer::render3D(float dt, uint64_t frameId) {
}

void Renderer::BloomRenderer::afterRender() {
    const GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    const GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (blendEnabled) {
        glDisable(GL_BLEND);
    }
    if (depthEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    bool horizontal = true, first_iteration = true;
    constexpr unsigned int amount = 10;
    shaderBlur->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur->setInt("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration) {
            first_iteration = false;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderBloomFinal->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    shaderBloomFinal->setInt("bloom", true);
    shaderBloomFinal->setFloat("exposure", 1.2f);
    renderQuad();

    if (depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    if (blendEnabled) {
        glEnable(GL_BLEND);
    }
}

void Renderer::BloomRenderer::renderQuad() {
    if (quadVAO == 0) {
        constexpr float quadVertices[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::BloomRenderer::beforeRender(const MODE mode) {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    constexpr GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffers(2, attachments);
    glClear(GL_DEPTH_BUFFER_BIT);
    this->mode = mode;
}

void Renderer::BloomRenderer::renderShadowMap() {}

void Renderer::BloomRenderer::resize(const int width, const int height, const glm::mat4 &projection) {
    if (width <= 0 || height <= 0) {
        return;
    }
    this->width = width;
    this->height = height;
    initializeFramebuffers();
}
