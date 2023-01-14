#include "EatRemoveAnimateRenderer.h"

namespace Renderer {

    EatRemoveAnimateRenderer::EatRemoveAnimateRenderer(Eat *eat, Camera *camera,
                                                       const glm::mat4 &projection, ResourceManager *resourceManager) :
            EatRenderer(eat, camera, projection, resourceManager) {
        mesh = resourceManager->getModel("coin")->getMesh();
        zoom = {0.013888889, 0.013888889, 0.013888889};
    }

    EatRemoveAnimateRenderer::~EatRemoveAnimateRenderer() {
        delete eat;
    }

    void EatRemoveAnimateRenderer::render() {

        if (eat && eat->isVisible()) {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", this->projection);
            baseShader->setInt("diffuseMap", 0);
            baseShader->setInt("normalMap", 1);
            baseShader->setInt("specularMap", 2);
            baseShader->setFloat("alpha", eat->getAlpha());
            baseShader->setBool("fogEnable", fog);

            // lighting info
            // -------------
            glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 26.3f);

            glLoadIdentity();

            texture1->bind(0);
            texture2->bind(1);
            texture3->bind(2);

            glm::vec3 position = eat->getPosition();
            const glm::vec4 *rotate = eat->getRotate();

            double now = glfwGetTime();
            float angle = rotate[1].x;
            if (now > lastTime + 0.0001) {
                angle++;
                position.z += 0.2;

                eat->setPosition(position);
                lastTime = now;
            }

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            // Transform the matrices to their correct form
            model = glm::translate(model, {0.0, 0.0, 0.0});
            model = glm::scale(model, {zoom.x, zoom.y, zoom.z});
            //model = glm::translate(model, {-25.0, -25.0, -23.0f}); // levy spodni okraj
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(90.0f), {1.0, 0.0, 0.0f});
            model = glm::rotate(model, glm::radians(angle), {0.0, 1.0, 0.0f});

            baseShader->setMat4("model", model);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setVec3("lightPos", lightPos);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

            eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);

            glActiveTexture(GL_TEXTURE0);

            eat->fadeStep(0.02f);
        }
    }

    void EatRemoveAnimateRenderer::beforeRender() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void EatRemoveAnimateRenderer::afterRender() {
        glDisable(GL_BLEND);
    }

    void EatRemoveAnimateRenderer::renderShadowMap() {
    }

} // Renderer