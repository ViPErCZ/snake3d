#include "SnakeRenderer.h"

namespace Renderer {

    SnakeRenderer::SnakeRenderer(Snake *snake, ShaderManager *shader, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager)
            : snake(snake), shader(shader), camera(camera), projection(projection), resourceManager(resManager) {
        mesh = resourceManager->getModel("cube")->getMesh();
        tilesCounter = (int)snake->getItems().size();
    }

    SnakeRenderer::~SnakeRenderer() {
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

        for (auto snakeTileIter = snake->getItems().begin(); snakeTileIter < snake->getItems().end(); snakeTileIter++) {
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

                mesh->bind();
                glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

                index++;
            }
        }
    }

    void SnakeRenderer::beforeRender() {
        tilesCounter = (int)snake->getItems().size();
    }

    void SnakeRenderer::afterRender() {
    }
} // Renderer