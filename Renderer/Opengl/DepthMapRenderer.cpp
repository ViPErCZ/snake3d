#include "DepthMapRenderer.h"

namespace Renderer {
    DepthMapRenderer::DepthMapRenderer(Camera *camera, glm::mat4 proj, ResourceManager *resManager) {
        resourceManager = resManager;
        this->camera = camera;
        this->projection = proj;
        shader = resourceManager->getShader("shadowShader");
        shader->use();
        shader->setMat4("projection", projection);
        shader->setInt("diffuseMap", 0);
        shader->setInt("shadowMap", 1);
        shader->setInt("normalMap", 2);
        shader->setInt("specularMap", 3);
        shader->setFloat("alpha", 1.0);

        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto texture = std::make_shared<TextureManager>();
        texture->addTexture(depthMap);
        resourceManager->addTexture("depth", texture);
        lightPos = {0.0f, 7.0f, 11.0f};

        // glm::mat4 lightProjection, lightView;
        // float near_plane = 1.0f, far_plane = 1.17549e-38f;
        // //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        // lightProjection = glm::ortho(-1.5f, 4.5f, -1.5f, 4.5f, 1.0f, 15.0f);
        // lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
        // lightSpaceMatrix = lightProjection * lightView;
        // const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        // simpleDepthShader->use();
        // simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    void DepthMapRenderer::beforeRender() {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void DepthMapRenderer::render(float dt) {
        // TODO: toto je tu jen proto, ze to nastavi lightPos, ale jeste vice dulezite lightSpaceMatrix
        shader->use();
        shader->setMat4("view", camera->getViewMatrix());
        shader->setVec3("viewPos", camera->getPosition());
        shader->setVec3("lightPos", lightPos);
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        const auto basicShader = resourceManager->getShader("basicShader");
        basicShader->use();
        basicShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    void DepthMapRenderer::renderShadowMap() {
    }

    void DepthMapRenderer::computeLightSpaceMatrix(shared_ptr<DirectionalLight> &light, glm::vec3 lightTarget, glm::vec3 sceneMin, glm::vec3 sceneMax) {
        glm::mat4 lightView = glm::lookAt(light.get()->getPosition(), lightTarget, glm::vec3(0,1,0));
        glm::vec3 corners[8] = {
            {sceneMin.x, sceneMin.y, sceneMin.z},
            {sceneMax.x, sceneMin.y, sceneMin.z},
            {sceneMin.x, sceneMax.y, sceneMin.z},
            {sceneMax.x, sceneMax.y, sceneMin.z},
            {sceneMin.x, sceneMin.y, sceneMax.z},
            {sceneMax.x, sceneMin.y, sceneMax.z},
            {sceneMin.x, sceneMax.y, sceneMax.z},
            {sceneMax.x, sceneMax.y, sceneMax.z}
        };

        glm::vec3 lightMin( FLT_MAX);
        glm::vec3 lightMax(-FLT_MAX);

        for (auto corner : corners) {
            glm::vec4 trf = lightView * glm::vec4(corner, 1.0f);
            lightMin = glm::min(lightMin, glm::vec3(trf));
            lightMax = glm::max(lightMax, glm::vec3(trf));
        }

        float near_plane = -lightMax.z;
        float far_plane  = -lightMin.z;

        auto lightProjection = glm::ortho(
            lightMin.x, lightMax.x,
            lightMin.y, lightMax.y,
            near_plane, far_plane
        );

        lightSpaceMatrix = lightProjection * lightView;

        const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    shared_ptr<Mesh> DepthMapRenderer::getMesh() {
        return nullptr;
    }

    void DepthMapRenderer::afterRender() {
//        auto debugDepthQuad = resourceManager->getShader("debugQuadShader");
//        debugDepthQuad->use();
//        debugDepthQuad->setFloat("near_plane", 1.0f);
//        debugDepthQuad->setFloat("far_plane", 7.5f);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, depthMap);
//        renderQuad();
    }

    void DepthMapRenderer::renderQuad() {
        if (quadVAO == 0) {
            constexpr float quadVertices[] = {
                    // positions        // texture Coords
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

} // Renderer