#include "SkyboxRenderer.h"

namespace Renderer {
    SkyboxRenderer::SkyboxRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection,
                                   ResourceManager *resourceManager) : cube(cube), camera(camera),
                                                                       projection(projection),
                                                                       resourceManager(resourceManager) {
        mesh = resourceManager->getModel("cube")->getMesh();
        shader = resourceManager->getShader("skyboxShader");
        texture = resourceManager->getTexture("skybox");
    }

    void SkyboxRenderer::render() {
        if (cube->isVisible()) {
            shader->use();
            shader->setInt("skybox", 0);

            glm::mat4 view = camera->getViewMatrix();
            view = glm::mat4(glm::mat3(camera->getViewMatrix())); // remove translation from the view matrix
            shader->setMat4("view", view);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setMat4("projection", projection);

            // skybox cube
            mesh->bind();
            texture->cubeBind();
            glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

    void SkyboxRenderer::beforeRender() {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    }

    void SkyboxRenderer::afterRender() {
        glDepthFunc(GL_LESS); // set depth function back to default
    }
} // Renderer