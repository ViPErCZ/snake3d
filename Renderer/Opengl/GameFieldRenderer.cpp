#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item, Camera *camera, glm::mat4 proj,
                                               ResourceManager *resManager) {
    gameField = item;
    this->item = *gameField->getTiles().begin();
    resourceManager = resManager;
    this->camera = camera;
    this->projection = proj;
    model = new GameFieldModel((*gameField->getTiles().begin()));
    baseShader = resourceManager->getShader("shadowShader").get();
    shadowShader = resourceManager->getShader("shadowDepthShader").get();
    texture1 = resourceManager->getTexture("gamefield.bmp").get();
    texture2 = resourceManager->getTexture("depth").get();
    texture3 = resourceManager->getTexture("gamefield_normal.jpg").get();
    texture4 = resourceManager->getTexture("gamefield_specular.jpg").get();
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
    delete model;
}

void Renderer::GameFieldRenderer::render(float dt) {
    baseShader->use();
    baseShader->setMat4("view", camera->getViewMatrix());
    baseShader->setMat4("projection", projection);
    baseShader->setVec3("viewPos", camera->getPosition());
    baseShader->setBool("shadowsEnable", shadow);
    baseShader->setBool("fogEnable", fog);
    baseShader->setInt("shadowMap", 4);
    baseShader->setInt("normalMap", 2);
    baseShader->setInt("specularMap", 3);

    if (!shadow) {
        glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.05f,  0.56f,  1.70f),
            glm::vec3( 1.19f,  0.56f,  1.70f),
        };

        // directional light
        baseShader->setVec3("dirLight.direction", -0.2f, -1.0f, -2.3f);
        baseShader->setVec3("dirLight.ambient", 0.005f, 0.005f, 0.05f);
        baseShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        baseShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        // baseShader->setInt("numPointLights", 1);
        baseShader->setFloat("material.shininess", 32.0f);
        baseShader->setInt("material.diffuse", 0);
        baseShader->setInt("material.specular", 1);
        // baseShader->setVec3("pointLights[0].position", pointLightPositions[0]);
        // baseShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        // baseShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        // baseShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        // baseShader->setFloat("pointLights[0].constant", 0.42f);
        // baseShader->setFloat("pointLights[0].linear", 1.0f);
        // baseShader->setFloat("pointLights[0].quadratic", 4.32f);
        // spotLight
        float time = glfwGetTime(); // nebo tvůj časový zdroj
        float baseCutOff = glm::radians(7.5f);
        float baseOuterCutOff = glm::radians(10.0f);
        float anglePulse = 0.5f + 0.5f * sin(time * 3.5f);

        float cutOff = glm::cos(baseCutOff + anglePulse * glm::radians(0.5f));
        float outerCutOff = glm::cos(baseOuterCutOff + anglePulse * glm::radians(0.5f));

        // ==== Pulzování ====
        float pulse = 0.7f
            + 0.1f * sin(time * 0.7f)
            + 0.05f * sin(time * 1.3f + 1.1f)
            + 0.03f * sin(time * 2.1f + 2.4f);

        // Výsledná intenzita mezi 0.6 – 0.9, s velmi plynulými změnami
        pulse = glm::clamp(pulse, 0.6f, 0.9f);

        const auto ambient = glm::vec3(0.08f * pulse);
        const auto diffuse = glm::vec3(0.85f * pulse);

        baseShader->setInt("numSpotLights", 1);
        baseShader->setVec3("spotLight[0].position", pointLightPositions[0]);
        baseShader->setVec3("spotLight[0].direction", glm::normalize(glm::vec3( -0.00f,  -0.32f,  -1.50f)));
        baseShader->setVec3("spotLight[0].ambient", ambient);
        baseShader->setVec3("spotLight[0].diffuse", diffuse);
        baseShader->setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
        baseShader->setFloat("spotLight[0].constant", 1.0f);
        baseShader->setFloat("spotLight[0].linear", 0.09f);
        baseShader->setFloat("spotLight[0].quadratic", 0.032f);
        baseShader->setFloat("spotLight[0].cutOff", cutOff);
        baseShader->setFloat("spotLight[0].outerCutOff", outerCutOff);
        baseShader->setVec3("spotLight[1].position", pointLightPositions[1]);
        baseShader->setVec3("spotLight[1].direction", glm::normalize(glm::vec3( 0.00f,  -0.32f,  -1.50f)));
        baseShader->setVec3("spotLight[1].ambient", ambient);
        baseShader->setVec3("spotLight[1].diffuse", diffuse);
        baseShader->setVec3("spotLight[1].specular", 1.0f, 1.0f, 1.0f);
        baseShader->setFloat("spotLight[1].constant", 1.0f);
        baseShader->setFloat("spotLight[1].linear", 0.09f);
        baseShader->setFloat("spotLight[1].quadratic", 0.032f);
        baseShader->setFloat("spotLight[1].cutOff", cutOff);
        baseShader->setFloat("spotLight[1].outerCutOff", outerCutOff);
        baseShader->setFloat("uTime", time);

        texture1->bind(0);
        texture4->bind(1);
    } else {
        texture1->bind(0);
        texture2->bindArr(4, 0);
        texture3->bind(2);
        texture4->bind(3);
    }

    renderScene(baseShader);
}

void Renderer::GameFieldRenderer::renderShadowMap() {
    shadowShader->use();
    // texture1->bind(0);
    renderScene(shadowShader);
}

void Renderer::GameFieldRenderer::renderScene(const ShaderManager *shader) const {
    int x = 0;
    glEnable(GL_DEPTH_TEST);

    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); ++Iter) {
        glLoadIdentity();
        if ((*Iter)->isVisible()) {

            glm::vec3 position = (*Iter)->getPosition();
            auto model = glm::mat4(1.0f);
            model = glm::scale(model, (*Iter)->getZoom());
            model = glm::translate(model, position);
            (*Iter)->setWorldMatrix(model);
            shader->setMat4("model", model);

            this->model->getMesh()->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(this->model->getMesh()->getIndices().size()), GL_UNSIGNED_INT, nullptr);
            x++;
        }
    }
}

shared_ptr<Mesh> Renderer::GameFieldRenderer::getMesh() {
    const std::shared_ptr<Mesh> meshShared(
        model->getMesh(), [](Mesh*) {
    });
    return meshShared;
}

void Renderer::GameFieldRenderer::beforeRender() {
}

void Renderer::GameFieldRenderer::afterRender() {
}
