#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item, ShaderManager* shader, Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
    gameField = item;
    resourceManager = resManager;
    this->shader = shader;
    this->camera = camera;
    this->projection = proj;
    model = new GameFieldModel((*gameField->getTiles().begin()));
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
    delete model;
}

void Renderer::GameFieldRenderer::render() {
    int x = 0;
    shader->use();
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("projection", this->projection);
    shader->setInt("diffuseMap", 0);
    shader->setInt("normalMap", 1);
    shader->setInt("specularMap", 2);
    shader->setFloat("alpha", 1.0);

    // lighting info
    // -------------
    glm::vec3 lightPos(camera->getPosition().x + 6, camera->getPosition().y + 6, -25.3f);

    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        glLoadIdentity();
        if ((*Iter)->isVisible()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("gamefield.bmp"));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("gamefield_normal.jpg"));
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("gamefield_specular.jpg"));

            glm::vec3 position = (*Iter)->getPosition();
            glm::vec3 zoom = (*Iter)->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);

            shader->setMat4("model", model);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setVec3("lightPos", lightPos);

            this->model->getVao().bind();
            glDrawElements(GL_TRIANGLES, (int)this->model->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            x++;
        }
    }
}

void Renderer::GameFieldRenderer::beforeRender() {
}

void Renderer::GameFieldRenderer::afterRender() {
}
