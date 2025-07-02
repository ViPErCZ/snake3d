#include "TorchRenderer.h"
#include <glm/ext/matrix_transform.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {
    TorchRenderer::TorchRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection,
                                 ResourceManager *resManager): cube(cube), camera(camera),
                                                                        projection(projection),
                                                                        resourceManager(resManager) {
        mesh = resourceManager->getModel("torch")->getMesh();
        baseShader = resourceManager->getShader("normalShader");
        texture = resourceManager->getTexture("torch.png");
        texture2 = resourceManager->getTexture("torch_normal.png");
    }

    TorchRenderer::~TorchRenderer() = default;

    void TorchRenderer::render(float dt) {
        baseShader->use();
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setInt("diffuseMap", 0);
        baseShader->setInt("normalMap", 1);
        baseShader->setInt("specularMap", 2);
        baseShader->setFloat("alpha", 1.0);
        baseShader->setVec3("viewPos", camera->getPosition());
        baseShader->setBool("parallaxEnable", false);
        baseShader->setBool("fogEnable", fog);
        texture->bind(0);
        texture2->bind(1);
        texture->bind(2);

        glm::vec3 lightPos(cube->getPosition().x, cube->getPosition().y - 6, cube->getPosition().z + 5);
        baseShader->setVec3("lightPos", lightPos);

        texture->bind(0);
        texture2->bind(1);
        texture->bind(2);

        renderScene(baseShader);
    }

    void TorchRenderer::renderShadowMap() {
    }

    void TorchRenderer::beforeRender() {
    }

    void TorchRenderer::afterRender() {
    }

    void TorchRenderer::renderScene(const ShaderManager *shader) {
        glLoadIdentity();

        glm::vec3 position = cube->getPosition();
        const glm::vec4 *rotate = cube->getRotate();

        // Initialize matrices
        glm::mat4 model = glm::mat4(1.0f);
        // Transform the matrices to their correct form
        model = glm::translate(model, {0.0, 0.0, 0.0});
        model = glm::scale(model, {0.12, 0.12, 0.12});
        //model = glm::translate(model, {-25.0, -25.0, -23.0f}); // levy spodni okraj
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(90.0f), {1.0, 0.0, 0.0f});

        shader->setMat4("model", model);

        mesh->bind();
        glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    }
} // Renderer
