#include "EatRenderer.h"

namespace Renderer {

    EatRenderer::EatRenderer(Eat *eat, ShaderManager *shaderManager, Camera *camera, const glm::mat4 &projection,
                             ResourceManager *resourceManager) : eat(eat), shaderManager(shaderManager), camera(camera),
                                                                 projection(projection),
                                                                 resourceManager(resourceManager) {
        model = new EatModel(eat);
    }

    void EatRenderer::render() {
        if (eat->isVisible()) {
            shaderManager->use();
            shaderManager->setMat4("view", camera->getViewMatrix());
            shaderManager->setMat4("projection", this->projection);
            shaderManager->setInt("diffuseMap", 0);
            shaderManager->setInt("normalMap", 1);
            shaderManager->setInt("specularMap", 2);
            shaderManager->setFloat("alpha", 0.1);

            // lighting info
            // -------------
            glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 26.3f);

            for (auto data: model->getMeshes()) {
                glLoadIdentity();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("Coin_Gold_albedo.png"));
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("Coin_Gold_nm.png"));
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("Coin_Gold_metalness.png"));

                glm::vec3 position = eat->getPosition();
                const glm::vec4 * rotate = eat->getRotate();

                double now = glfwGetTime();
                float angle = rotate[1].x;
                if (now > lastTime + 0.005) {
                    angle++;
                    lastTime = now;
                }

                // Initialize matrices
                glm::mat4 model = glm::mat4(1.0f);
                // Transform the matrices to their correct form
                model = glm::translate(model, {0.0, 0.0, 0.0});
                model = glm::scale(model, {0.013888889, 0.013888889, 0.013888889});
                //model = glm::translate(model, {-25.0, -25.0, -23.0f}); // levy spodni okraj
                model = glm::translate(model, position);
                model = glm::rotate(model, glm::radians(90.0f), {1.0, 0.0, 0.0f});
                model = glm::rotate(model, glm::radians(angle), {0.0, 1.0, 0.0f});

                shaderManager->setMat4("model", model);
                shaderManager->setVec3("viewPos", camera->getPosition());
                shaderManager->setVec3("lightPos", lightPos);

                data.first->bind();
                glDrawElements(GL_TRIANGLES, (int)data.second->getIndices().size(), GL_UNSIGNED_INT, nullptr);

                eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);
            }

        }

        glActiveTexture(GL_TEXTURE0);
    }

    void EatRenderer::beforeRender() {
    }

    void EatRenderer::afterRender() {
    }
} // Renderer