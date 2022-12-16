#include "EatRenderer.h"

namespace Renderer {

    EatRenderer::EatRenderer(Eat *eat, Camera *camera, const glm::mat4 &projection,
                             ResourceManager *resourceManager) : eat(eat), camera(camera),
                                                                 projection(projection),
                                                                 resourceManager(resourceManager) {
        mesh = resourceManager->getModel("coin")->getMesh();
        baseShader = resourceManager->getShader("normalShader");
        shadowShader = resourceManager->getShader("shadowDepthShader");
        texture1 = resourceManager->getTexture("Coin_Gold_albedo.png");
        texture2 = resourceManager->getTexture("Coin_Gold_nm.png");
        texture3 = resourceManager->getTexture("Coin_Gold_metalness.png");
        depthTexture = resourceManager->getTexture("depth");
    }

    void EatRenderer::render() {
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setInt("diffuseMap", 0);
        baseShader->setInt("normalMap", 1);
        baseShader->setInt("specularMap", 2);
        baseShader->setFloat("alpha", 1.0);
        baseShader->setVec3("viewPos", camera->getPosition());
        texture1->bind(0);
        texture2->bind(1);
        texture3->bind(2);
        renderScene(baseShader);
    }

    void EatRenderer::renderShadowMap() {
        depthTexture->bind(0);
        renderScene(shadowShader);
    }

    void EatRenderer::renderScene(ShaderManager *shader) {
        if (eat->isVisible()) {
            shader->use();
            glLoadIdentity();

            glm::vec3 position = eat->getPosition();
            const glm::vec4 *rotate = eat->getRotate();

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

            shader->setMat4("model", model);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

            eat->setRotate(rotate[0], {angle, 0, 1, 0}, rotate[2]);

        }

        glActiveTexture(GL_TEXTURE0);
    }

    void EatRenderer::beforeRender() {
    }

    void EatRenderer::afterRender() {
    }
}