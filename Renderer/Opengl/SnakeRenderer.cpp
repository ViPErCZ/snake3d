#include "SnakeRenderer.h"

namespace Renderer {

    SnakeRenderer::SnakeRenderer(Snake *snake, Camera *camera, const glm::mat4 &projection, ResourceManager *resManager)
            : snake(snake), camera(camera), projection(projection), resourceManager(resManager), blur(false), renderStyle(2) {
        mesh = (*resourceManager->getAnimationModel("tile")->getMeshes().begin());
        baseShader = resourceManager->getShader("basicShader");
        respawn = resourceManager->getShader("respawnShader");
        shadowShader = resourceManager->getShader("shadowDepthShader");
        shaderLight = resourceManager->getShader("bloomLight");
        snakeTileTexture = resourceManager->getTexture("snake.bmp");
        snakeHeadTexture = resourceManager->getTexture("head.bmp");
        noise = resourceManager->getTexture("fast_noise.bmp");
        startTime = glfwGetTime();
    }

    SnakeRenderer::~SnakeRenderer() {
        delete snake;
    }

    void SnakeRenderer::render(float dt) {
        if (blur) {
            shaderLight->use();
            shaderLight->setMat4("projection", projection);
            shaderLight->setMat4("view", camera->getViewMatrix());
            renderScene(shaderLight);
        } else {
            const double elapsed = glfwGetTime() - startTime;
            respawn->use();
            respawn->setMat4("view", camera->getViewMatrix());
            respawn->setMat4("projection", projection);
            respawn->setVec3("viewPos", camera->getPosition());
            respawn->setInt("u_NoiseTexture", 0);
            respawn->setVec4("u_LightColor", glm::vec4(1.000000, 0.898039, 0.100000, 1.0f));
            respawn->setFloat("u_Speed", 2.0f);
            respawn->setFloat("u_Delay", 1.0f); // zpoždění 1 sekundy před startem animace
            respawn->setFloat("u_FloatParameter", 0.1);
            respawn->setFloat("u_Time", static_cast<float>(elapsed));
            respawn->setBool("useBones", false);
            respawn->setBool("useMaterial", true);
            noise->bind();
            renderScene(respawn);
        }
    }

    void SnakeRenderer::renderShadowMap() {
        shadowShader->use();
        renderScene(shadowShader);
    }

    void SnakeRenderer::renderScene(const ShaderManager *shader) const {
        for (auto snakeTileIter = snake->getItems().end()-1; snakeTileIter >= snake->getItems().begin(); --snakeTileIter) {
            if ((*snakeTileIter)->tile->isVisible()) {
                glLoadIdentity();

                if (snakeTileIter == this->snake->getItems().begin()) {
                    snakeTileTexture->bind();
                } else {
                    if (renderStyle == 2) {
                        baseShader->setBool("useMaterial", true);
                    } else {
                        snakeHeadTexture->bind();
                    }
                }

                glm::vec3 position = (*snakeTileIter)->tile->getPosition();
                auto model = glm::mat4(1.0f);
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
                        shader->setVec3("lightColor", {5.0f, 2.0f, 0.0f});
                    } else {
                        shader->setVec3("lightColor", {2.0f, 2.0f, 0.0f});
                    }
                }
                mesh->bind();

                glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    void SnakeRenderer::beforeRender() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_BACK);
    }

    void SnakeRenderer::afterRender() {
        glDisable(GL_DEPTH_TEST);
    }

    void SnakeRenderer::toggleBlur() {
        blur = !blur;
    }

    void SnakeRenderer::toggleStyle(const int style) {
        if (style == 1) {
            mesh = resourceManager->getModel("cube")->getMesh();
            renderStyle = 1;
        } else {
            mesh = (*resourceManager->getAnimationModel("tile")->getMeshes().begin());
            renderStyle = 2;
        }
    }
} // Renderer