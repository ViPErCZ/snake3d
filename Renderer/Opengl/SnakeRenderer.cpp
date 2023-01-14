#include "SnakeRenderer.h"

namespace Renderer {

    SnakeRenderer::SnakeRenderer(Snake *snake, Camera *camera, const glm::mat4 &projection, ResourceManager *resManager)
            : snake(snake), camera(camera), projection(projection), resourceManager(resManager), blur(false) {
        mesh = resourceManager->getModel("cube")->getMesh();
        baseShader = resourceManager->getShader("basicShader");
        shadowShader = resourceManager->getShader("shadowDepthShader");
        shaderLight = resourceManager->getShader("bloomLight");
        snakeTileTexture = resourceManager->getTexture("snake.bmp");
        snakeHeadTexture = resourceManager->getTexture("head.bmp");
    }

    SnakeRenderer::~SnakeRenderer() {
        delete snake;
    }

    void SnakeRenderer::render() {
        if (blur) {
            shaderLight->use();
            shaderLight->setMat4("projection", projection);
            shaderLight->setMat4("view", camera->getViewMatrix());
            renderScene(shaderLight);
        } else {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setVec3("viewPos", camera->getPosition());
            renderScene(baseShader);
        }
        baseShader->setBool("fogEnable", fog);
    }

    void SnakeRenderer::renderShadowMap() {
        shadowShader->use();
        renderScene(shadowShader);
    }

    void SnakeRenderer::renderScene(ShaderManager *shader) {
        for (auto snakeTileIter = snake->getItems().end()-1; snakeTileIter >= snake->getItems().begin(); snakeTileIter--) {
            if ((*snakeTileIter)->tile->isVisible()) {
                glLoadIdentity();

                if (snakeTileIter == this->snake->getItems().begin()) {
                    snakeTileTexture->bind();
                } else {
                    snakeHeadTexture->bind();
                }

                glm::vec3 position = (*snakeTileIter)->tile->getPosition();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, {0.0, 0.0, 0.0});
                if (snakeTileIter == this->snake->getItems().begin()) {
                    model = glm::scale(model, {0.041667f, 0.041667f, 0.041667f});
                } else {
                    model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
                }
                model = glm::translate(model, position);

                shader->setMat4("model", model);
                if (blur) {
                    if (snakeTileIter == this->snake->getItems().begin()) {
                        shader->setVec3("lightColor", {10.0f, 0.0f, 0.0f});
                    } else {
                        shader->setVec3("lightColor", {0.0f, 5.0f, 0.0f});
                    }
                }
                mesh->bind();

                glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    void SnakeRenderer::beforeRender() {
    }

    void SnakeRenderer::afterRender() {
    }

    void SnakeRenderer::toggleBlur() {
        blur = !blur;
    }
} // Renderer