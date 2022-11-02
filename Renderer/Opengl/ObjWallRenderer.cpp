#include "ObjWallRenderer.h"

namespace Renderer {
    ObjWallRenderer::ObjWallRenderer(ObjWall *item, ShaderManager* shader, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : shaderManager(shader), wall(item), resourceManager(resManager) {
        mesh = resourceManager->getModel("cube")->getMesh();
        this->camera = camera;
        projection = proj;
    }

    ObjWallRenderer::~ObjWallRenderer() {
        delete wall;
    }

    void ObjWallRenderer::render() {
        int index = 0;
        shaderManager->use();
        shaderManager->setMat4("view", camera->getViewMatrix());
        shaderManager->setMat4("projection", this->projection);
        shaderManager->setInt("diffuseMap", 0);
        shaderManager->setInt("normalMap", 1);
        shaderManager->setInt("specularMap", 2);
        shaderManager->setFloat("alpha", 1.0);


        // lighting info
        // -------------
        glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 26.3f);

        for (auto item: wall->getItems()) {
            glLoadIdentity();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("brickwork-texture.jpg"));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("brickwork_normal-map.jpg"));
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("brickwork-bump-map.jpg"));

            glm::vec3 position = item->getPosition();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            // Transform the matrices to their correct form
            model = glm::translate(model, {0.0, 0.0, 0.0});
            model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
            //model = glm::translate(model, {-25.0, -25.0, -23.0f}); // levy spodni okraj
            model = glm::translate(model, position);

            shaderManager->setMat4("model", model);
            shaderManager->setVec3("viewPos", camera->getPosition());
            shaderManager->setVec3("lightPos", lightPos);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
        }

        glEnable(GL_TEXTURE0);
    }

    void ObjWallRenderer::beforeRender() {
    }

    void ObjWallRenderer::afterRender() {
    }

} // Renderer