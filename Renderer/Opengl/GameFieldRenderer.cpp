#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
    gameField = item;
    resourceManager = resManager;
    this->camera = camera;
    this->projection = proj;
    model = new GameFieldModel((*gameField->getTiles().begin()));
    shader = resourceManager->getShader("shadowShader");
    texture1 = resourceManager->getTexture("gamefield.bmp");
    texture2 = resourceManager->getTexture("depth");
    shadow = false;
//    texture2 = resourceManager->getTexture("gamefield_normal.jpg");
//    texture3 = resourceManager->getTexture("gamefield_specular.jpg");
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
    delete model;
}

void Renderer::GameFieldRenderer::render() {
    if (!shadow) {
        shader = resourceManager->getShader("shadowDepthShader");
    } else {
        shader = resourceManager->getShader("shadowShader");
    }
    int x = 0;
    shader->use();
    if (shadow) {
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", projection);
        shader->setVec3("viewPos", camera->getPosition());
//        glm::vec3 lightPos = {0, -40, -1};
//        shader->setVec3("lightPos", lightPos);
    }
//    shader->setInt("diffuseMap", 0);
//    shader->setInt("normalMap", 1);
//    shader->setInt("specularMap", 2);
//    shader->setFloat("alpha", 1.0);

    // lighting info
    // -------------
//    glm::vec3 lightPos(camera->getPosition().x + 6, camera->getPosition().y + 6, -25.3f);

    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        glLoadIdentity();
        if ((*Iter)->isVisible()) {
            if (!shadow) {
                texture1->bind(0);
            } else {
                texture1->bind(0);
                texture2->bind(1);
            }
//            texture2->bind(1);
//            texture3->bind(2);

            glm::vec3 position = (*Iter)->getPosition();
            glm::vec3 zoom = (*Iter)->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            shader->setMat4("model", model);

            this->model->getMesh()->bind();
            glDrawElements(GL_TRIANGLES, (int)this->model->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            x++;
        }
    }
}

void Renderer::GameFieldRenderer::beforeRender() {
}

void Renderer::GameFieldRenderer::afterRender() {
}
