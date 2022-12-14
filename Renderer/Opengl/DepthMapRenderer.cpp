#include "DepthMapRenderer.h"

namespace Renderer {
    DepthMapRenderer::DepthMapRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
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

        const unsigned int SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;
        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
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

        float planeVertices[] = {
                // positions            // normals         // texcoords
                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
                -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
                -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

                25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
                -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
                25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
        };
        // plane VAO
        unsigned int planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        glm::mat4 lightProjection, lightView;
        //glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 17.5f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    void DepthMapRenderer::beforeRender() {
        glViewport(0, 0, 1024*4, 1024*4);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void DepthMapRenderer::render() {
        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
//        baseShader->use();
//        baseShader->setVec3("viewPos", camera->getPosition());
//        baseShader->setVec3("lightPos", lightPos);
//        baseShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
//
//        auto debugDepthQuad = resourceManager->getShader("debugQuadShader");
//        debugDepthQuad->use();
//        debugDepthQuad->setFloat("near_plane", 1.0f);
//        debugDepthQuad->setFloat("far_plane", 7.5f);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, depthMap);
////        renderQuad();

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------



        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        shader->use();
        //glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), (float)1920 / (float)1080, 0.1f, 100.0f);

        shader->setMat4("view", camera->getViewMatrix());
        // set light uniforms
        shader->setVec3("viewPos", camera->getPosition());
        shader->setVec3("lightPos", lightPos);
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

//        resourceManager->getTexture("gamefield.bmp")->bind(0);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, depthMap);
//        renderScene(baseShader);


// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------


//        auto mesh = resourceManager->getModel("cube")->getMesh();
//        auto basicShader = resourceManager->getShader("basicShader");
//
//        basicShader->use();
//
//        glm::mat4 model = glm::mat4(1.0f);
//        // Transform the matrices to their correct form
//        model = glm::translate(model, {0.0, 0.0, 0.0});
//        model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
//        //model = glm::translate(model, {-25.0, -25.0, -23.0f}); // levy spodni okraj
//        model = glm::translate(model, lightPos);
//
//        basicShader->setMat4("model", model);
//        mesh->bind();
//        resourceManager->getTexture("head.bmp")->bind();
//        glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    }

    void DepthMapRenderer::renderShadowMap() {

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

    void DepthMapRenderer::renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

} // Renderer