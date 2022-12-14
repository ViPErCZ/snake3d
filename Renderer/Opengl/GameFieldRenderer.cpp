#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item, Camera *camera, glm::mat4 proj,
                                               ResourceManager *resManager) {
    gameField = item;
    resourceManager = resManager;
    this->camera = camera;
    this->projection = proj;
    model = new GameFieldModel((*gameField->getTiles().begin()));
    baseShader = resourceManager->getShader("shadowShader");
    shadowShader = resourceManager->getShader("shadowDepthShader");
    texture1 = resourceManager->getTexture("gamefield.bmp");
    texture2 = resourceManager->getTexture("depth");
    texture3 = resourceManager->getTexture("gamefield_normal.jpg");
    texture4 = resourceManager->getTexture("gamefield_specular.jpg");
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
    delete model;
}

void Renderer::GameFieldRenderer::render() {
    baseShader->use();
    baseShader->setMat4("view", camera->getViewMatrix());
    baseShader->setMat4("projection", projection);
    baseShader->setVec3("viewPos", camera->getPosition());
    baseShader->setBool("shadowsEnable", shadow);

    if (!shadow) {
        glm::vec3 lightPos(camera->getPosition().x + 6, camera->getPosition().y + 6, -1.3f);
        baseShader->setVec3("lightPos", lightPos);
    }

    texture1->bind(0);
    texture2->bind(1);
    texture3->bind(2);
    texture4->bind(3);

    renderScene(baseShader);
}

void Renderer::GameFieldRenderer::renderShadowMap() {
    shadowShader->use();
    texture1->bind(0);
    renderScene(shadowShader);
}

void Renderer::GameFieldRenderer::renderScene(ShaderManager *shader) {
    int x = 0;

    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        glLoadIdentity();
        if ((*Iter)->isVisible()) {

            glm::vec3 position = (*Iter)->getPosition();
            glm::vec3 zoom = (*Iter)->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            shader->setMat4("model", model);

            this->model->getMesh()->bind();
            glDrawElements(GL_TRIANGLES, (int) this->model->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            x++;
        }
    }
}

void Renderer::GameFieldRenderer::beforeRender() {
}

void Renderer::GameFieldRenderer::afterRender() {
}
