#include "ObjWallRenderer.h"

namespace Renderer {
    ObjWallRenderer::ObjWallRenderer(ObjWall *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : wall(item), resourceManager(resManager) {
        mesh = resourceManager->getModel("cube")->getMesh();
        shader = resourceManager->getShader("normalShader");
        texture1 = resourceManager->getTexture("brickwork-texture.jpg");
        texture2 = resourceManager->getTexture("brickwork_normal-map.jpg");
        texture3 = resourceManager->getTexture("brickwork-bump-map.jpg");
        this->camera = camera;
        projection = proj;
    }

    ObjWallRenderer::~ObjWallRenderer() {
        delete wall;
    }

    void ObjWallRenderer::render() {
        shader->use();
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", this->projection);
        shader->setInt("diffuseMap", 0);
        shader->setInt("normalMap", 1);
        shader->setInt("specularMap", 2);
        shader->setFloat("alpha", 1.0);

        // lighting info
        // -------------
        glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 36.3f);

        for (auto item: wall->getItems()) {
            glLoadIdentity();

            texture1->bind(0);
            texture2->bind(1);
            texture3->bind(2);

            glm::vec3 position = item->getPosition();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);
            // Transform the matrices to their correct form
            model = glm::translate(model, {0.0, 0.0, 0.0});
            model = glm::scale(model, {0.041666667f, 0.041666667f, 0.041666667f});
            model = glm::translate(model, position);

            shader->setMat4("model", model);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setVec3("lightPos", lightPos);
            shader->setBool("shadowEnable", true);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
        }

        glEnable(GL_TEXTURE0);
    }

    void ObjWallRenderer::beforeRender() {
        if (shadow) {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    void ObjWallRenderer::afterRender() {
        if (shadow) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(3.0f, 3.0f);
        }
    }

    void ObjWallRenderer::renderShadowMap() {

    }

} // Renderer