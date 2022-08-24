#include "SnakeRenderer.h"

namespace Renderer {

    SnakeRenderer::SnakeRenderer(Snake *snake, ShaderManager *shader, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager)
            : snake(snake), shader(shader), camera(camera), projection(projection), resourceManager(resManager) {
        model = new SnakeModel(snake);
        tilesCounter = (int)snake->getItems().size();
    }

    SnakeRenderer::~SnakeRenderer() {
        delete model;
        delete snake;
    }

    void SnakeRenderer::render() {
        int index = 0;
        shader->use();
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", this->projection);
        shader->setInt("diffuseMap", 0);
        shader->setInt("normalMap", 1);
        shader->setInt("specularMap", 2);
        shader->setFloat("alpha", 1.0);

        for (auto data: model->getMeshes()) {
            auto snakeTileIter = this->snake->getItems().begin() + index;
            if ((*snakeTileIter)->tile->isVisible()) {
                glLoadIdentity();

                glActiveTexture(GL_TEXTURE0);
                if (snakeTileIter == this->snake->getItems().begin()) {
                    glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("snake.bmp"));
                } else {
                    glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("head.bmp"));
                }

                glm::vec3 position = (*snakeTileIter)->tile->getPosition();

                // Initialize matrices
                glm::mat4 model = glm::mat4(1.0f);
                // Transform the matrices to their correct form
                model = glm::translate(model, {0.0, 0.0, 0.0});
                model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
                model = glm::translate(model, position);

                shader->setMat4("model", model);

                data.first->bind();
                glDrawElements(GL_TRIANGLES, (int)data.second->getIndices().size(), GL_UNSIGNED_INT, nullptr);

                index++;
            }
        }
    }

    void SnakeRenderer::beforeRender() {
        int currentSize = (int)snake->getItems().size();
        if (tilesCounter < currentSize) {
            model->addTiles(currentSize - tilesCounter);
            tilesCounter = currentSize;
        } else if (tilesCounter > currentSize) {
            delete model;
            model = new SnakeModel(snake);
            tilesCounter = currentSize;
        }
    }

    void SnakeRenderer::afterRender() {
    }
} // Renderer