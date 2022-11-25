#include "DepthMapRenderer.h"

namespace Renderer {
    DepthMapRenderer::DepthMapRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
        resourceManager = resManager;
        this->camera = camera;
        this->projection = proj;
        shader = resourceManager->getShader("shadowShader");
        shader->use();
        shader->setInt("diffuseTexture", 0);
        shader->setInt("shadowMap", 1);

        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 768;
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
        test = 0;
        lightPos = {-2.0f, 6.0f, 0.0f};

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
        float near_plane = 1.0f, far_plane = 7.5f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(1.0f, 0.0f, -23.0f), glm::vec3(0.0, 0.0, 1.0));
        lightSpaceMatrix = lightProjection * lightView;
        auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    void DepthMapRenderer::beforeRender() {
//        auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
//        // lighting info
//        // -------------
//        // change light position over time
//        lightPos.x = sin(glfwGetTime()) * 3.0f;
//        lightPos.z = cos(glfwGetTime()) * 2.0f;
//        lightPos.y = 5.0 + cos(glfwGetTime()) * 1.0f;
//        // 1. render depth of scene to texture (from light's perspective)
//        // --------------------------------------------------------------
//        glm::mat4 lightProjection, lightView;
//        float near_plane = 1.0f, far_plane = 7.5f;
//        lightProjection = glm::perspective(glm::radians(0.0f), (GLfloat)1024 / (GLfloat)768, near_plane, far_plane);
////        lightProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, near_plane, far_plane);
////        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0));
//        lightSpaceMatrix = lightProjection * lightView;
//        // render scene from light's point of view
//        simpleDepthShader->use();
//        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
//
//        glViewport(0, 0, 1024, 768);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glClear(GL_DEPTH_BUFFER_BIT);


        // render scene from light's point of view
        auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");

        glViewport(0, 0, 1024, 768);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        resourceManager->getTexture("gamefield.bmp")->bind(0);
        renderScene(simpleDepthShader);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        // reset viewport
//        glViewport(0, 0, 1920, 1080);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DepthMapRenderer::render() {
        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
//        shader->use();
//        shader->setVec3("viewPos", camera->getPosition());
//        shader->setVec3("lightPos", lightPos);
//        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
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
        glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), (float)1920 / (float)1080, 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        // set light uniforms
        shader->setVec3("viewPos", camera->getPosition());
        shader->setVec3("lightPos", lightPos);
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
//        resourceManager->getTexture("gamefield.bmp")->bind(0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(shader);


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

    void DepthMapRenderer::afterRender() {
        test -= 0.1;

        auto debugDepthQuad = resourceManager->getShader("debugQuadShader");
        debugDepthQuad->use();
        debugDepthQuad->setFloat("near_plane", 1.0f);
        debugDepthQuad->setFloat("far_plane", 7.5f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
//        renderQuad();
    }

    void DepthMapRenderer::renderScene(const ShaderManager *shader)
    {
        // floor
//        glm::mat4 model = glm::mat4(1.0f);
//        shader->setMat4("model", model);
//        glBindVertexArray(planeVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // cubes
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader->setMat4("model", model);
//        renderCube();
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, lightPos);
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader->setMat4("model", model);
//        renderCube();
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
//        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        model = glm::scale(model, glm::vec3(0.25));
//        shader->setMat4("model", model);
//        renderCube();
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

    void DepthMapRenderer::renderCube()
    {
        // initialize (if necessary)
        if (cubeVAO == 0)
        {
            float vertices[] = {
                    // back face
                    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                    1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                    1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                    1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                    // front face
                    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                    1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                    1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                    1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                    // left face
                    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                    // right face
                    1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                    1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                    1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                    1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                    1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                    1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                    // bottom face
                    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                    1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                    // top face
                    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                    1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                    1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                    1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

} // Renderer