#include "SkyboxRenderer.h"

namespace Renderer {
    SkyboxRenderer::SkyboxRenderer(shared_ptr<Cube> cube, const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                                   const shared_ptr<ResourceManager> &resourceManager)
        : cube(std::move(cube)), camera(camera), projection(projection), resourceManager(resourceManager) {
        mesh = resourceManager->getModel("cube");
        shader = resourceManager->getShader("skyboxShader");
        texture = resourceManager->getTexture("skybox");
    }

    void SkyboxRenderer::render3D(float dt, uint64_t frameId) {
        if (cube->isVisible()) {
            shader->use();
            shader->setInt("skybox", 0);

            const auto view = glm::mat4(glm::mat3(camera->getViewMatrix()));
            shader->setMat4("view", view);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setMat4("projection", projection);

            // skybox cube
            mesh->bind();
            texture->cubeBind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

    void SkyboxRenderer::beforeRender(const MODE mode) {
        glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);
        this->mode = mode;
    }

    void SkyboxRenderer::afterRender() {
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }
} // Renderer