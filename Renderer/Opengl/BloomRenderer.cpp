#include "BloomRenderer.h"
#include "../../Resource/ShaderLoader.h"

Renderer::BloomRenderer::BloomRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
    resourceManager = resManager;
    this->camera = camera;
    this->projection = proj;

    resourceManager->addShader("bloom", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/bloom/bloom.vs", "Assets/Shaders/bloom/bloom.fs")));
    resourceManager->addShader("bloomLight", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/bloom/bloom.vs", "Assets/Shaders/bloom/light.fs")));
    resourceManager->addShader("blur", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/bloom/blur.vs", "Assets/Shaders/bloom/blur.fs")));
    resourceManager->addShader("bloomFinal", std::make_shared<ShaderManager>(
            ShaderLoader::loadShader("Assets/Shaders/bloom/bloom_final.vs", "Assets/Shaders/bloom/bloom_final.fs")));

    shader = resourceManager->getShader("bloom");
    shaderLight = resourceManager->getShader("bloomLight");
    shaderBlur = resourceManager->getShader("blur");
    shaderBloomFinal = resourceManager->getShader("bloomFinal");

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // lighting info
    // -------------
    // positions
    lightPositions.emplace_back( 23.0f, -3.0f,  -23.0f);
    //lightPositions.emplace_back(-4.0f, 0.5f, -3.0f);
    //lightPositions.emplace_back( 3.0f, 0.5f,  1.0f);
    //lightPositions.emplace_back(-.8f,  2.4f, -1.0f);
    // colors
    //lightColors.emplace_back(5.0f,   5.0f,  5.0f); // bila
    lightColors.emplace_back(10.0f,  0.0f,  0.0f); // cervena
    //lightColors.emplace_back(0.0f,   0.0f,  15.0f); // modra
    //lightColors.emplace_back(0.0f,   5.0f,  0.0f); // zelena

    shader->use();
    shader->setInt("diffuseTexture", 0);
    shaderBlur->use();
    shaderBlur->setInt("image", 0);
    shaderBloomFinal->use();
    shaderBloomFinal->setInt("scene", 0);
    shaderBloomFinal->setInt("bloomBlur", 1);
}

void Renderer::BloomRenderer::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    //resourceManager->getTexture("gamefield.bmp")->bind(0);
    // set lighting uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        shader->setVec3("lights[" + std::to_string(i) + "].Position", camera->getStickyPosition());
        shader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
    }
    shader->setVec3("viewPos", camera->getPosition());
    // create one large cube that acts as the floor
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
//    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
//    shader->setMat4("model", model);
//    renderCube();
//    // then create multiple cubes as the scenery
//    resourceManager->getTexture("gamefield.bmp")->bind(0);
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
//    model = glm::scale(model, glm::vec3(0.5f));
//    shader->setMat4("model", model);
//    renderCube();
//
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
//    model = glm::scale(model, glm::vec3(0.5f));
//    shader->setMat4("model", model);
//    renderCube();
//
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
//    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//    shader->setMat4("model", model);
//    renderCube();
//
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
//    model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//    model = glm::scale(model, glm::vec3(1.25));
//    shader->setMat4("model", model);
//    renderCube();
//
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
//    model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//    shader->setMat4("model", model);
//    renderCube();
//
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
//    model = glm::scale(model, glm::vec3(0.5f));
//    shader->setMat4("model", model);
//    renderCube();

    // finally show all the light sources as bright cubes
    shaderLight->use();
    shaderLight->setMat4("projection", projection);
    shaderLight->setMat4("view", view);

    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, {0.0, 0.0, 0.0});
        model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
        model = glm::translate(model, camera->getStickyPosition());
        //model = glm::scale(model, glm::vec3(0.25f));
        shaderLight->setMat4("model", model);
        shaderLight->setVec3("lightColor", lightColors[i]);
        auto mesh = resourceManager->getModel("cube")->getMesh();

        mesh->bind();
        glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BloomRenderer::afterRender() {
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    shaderBlur->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur->setInt("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
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
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    shaderBloomFinal->setInt("bloom", true);
    shaderBloomFinal->setFloat("exposure", 1.2f);
    renderQuad();
}

void Renderer::BloomRenderer::renderQuad() {
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

void Renderer::BloomRenderer::renderShadowMap() {

}

void Renderer::BloomRenderer::beforeRender() {

}
