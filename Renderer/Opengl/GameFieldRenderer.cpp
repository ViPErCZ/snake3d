#include "GameFieldRenderer.h"

Renderer::GameFieldRenderer::GameFieldRenderer(GameField *item, ShaderManager* shader, Camera* camera, glm::mat4 proj) {
    gameField = item;
    this->shader = shader;
    this->camera = camera;
    this->projection = proj;
    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        auto field = new GameFieldModel((*Iter));
        model.push_back(field);
    }
}

Renderer::GameFieldRenderer::~GameFieldRenderer() {
    delete gameField;
    for (auto Iter = model.begin(); Iter < model.end(); Iter++) {
        delete (*Iter);
    }
    model.clear();
}

void Renderer::GameFieldRenderer::render() {
    int x = 0;
    shader->use();
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("projection", this->projection);
    shader->setInt("diffuseMap", 0);
    shader->setInt("normalMap", 1);
    shader->setInt("specularMap", 2);

    // lighting info
    // -------------
    glm::vec3 lightPos(1.2f, 3.0f, 2.3f);

    for (auto Iter = gameField->getTiles().begin(); Iter < gameField->getTiles().end(); Iter++) {
        glLoadIdentity();
        if ((*Iter)->isVisible()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 6);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 16);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 4);

            glm::vec3 position = (*Iter)->getPosition();
            glm::vec3 zoom = (*Iter)->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            // Transform the matrices to their correct form
            //model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0, 0.0f, 0.0f));
//            model = glm::rotate(model, rotate[1].x, glm::vec3(0.0, 1.0f, 0.0f));
            //model = glm::rotate(model, glm::radians(41.0f), glm::vec3(0.0, 0.0f, 1.0f));
            //model = glm::scale(model, {2.0f, 2.0f, 0.0f});
            model = glm::translate(model, position);

            shader->setMat4("model", model);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setVec3("lightPos", lightPos);

            auto modelIter = this->model.begin() + x;

            (*modelIter)->getVao().bind();
            glDrawElements(GL_TRIANGLES, (int)(*modelIter)->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            x++;
        }
    }
}

void Renderer::GameFieldRenderer::beforeRender() {
}

void Renderer::GameFieldRenderer::afterRender() {
}
