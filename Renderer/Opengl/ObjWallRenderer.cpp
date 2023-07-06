#include "ObjWallRenderer.h"

namespace Renderer {
    ObjWallRenderer::ObjWallRenderer(ObjWall *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager)
        : wall(item), resourceManager(resManager), camera(camera), projection(proj), parallax(false), heightScale(0.1f) {
        mesh = resourceManager->getModel("cube")->getMesh();
        shader = resourceManager->getShader("normalShader");
        texture1 = resourceManager->getTexture("brickwork-texture.jpg");
        texture2 = resourceManager->getTexture("brickwork_normal-map.jpg");
        texture3 = resourceManager->getTexture("brickwork-bump-map.jpg");
        texture4 = resourceManager->getTexture("bricks2_disp.jpg");
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
        shader->setInt("depthMap", 2);
        shader->setBool("parallaxEnable", parallax);
        shader->setFloat("alpha", 1.0);
        shader->setFloat("heightScale", heightScale);
        shader->setBool("fogEnable", fog);

        // lighting info
        // -------------
        glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 36.3f);

        for (auto item: wall->getItems()) {
            glLoadIdentity();

            if (parallax) {
                texture1->bind(0);
                texture2->bind(1);
                texture4->bind(2);
            } else {
                texture1->bind(0);
                texture2->bind(1);
                texture3->bind(2);
            }

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
        glEnable(GL_DEPTH_TEST);
    }

    void ObjWallRenderer::afterRender() {
    }

    void ObjWallRenderer::renderShadowMap() {

    }

    void ObjWallRenderer::toggleParallax() {
        ObjWallRenderer::parallax = !parallax;

        if (parallax) {
            texture1 = resourceManager->getTexture("bricks2.jpg");
            texture2 = resourceManager->getTexture("bricks2_normal.jpg");
            texture4 = resourceManager->getTexture("bricks2_disp.jpg");
        } else {
            texture1 = resourceManager->getTexture("brickwork-texture.jpg");
            texture2 = resourceManager->getTexture("brickwork_normal-map.jpg");
            texture3 = resourceManager->getTexture("brickwork-bump-map.jpg");
        }
    }

    void ObjWallRenderer::downScale() {
        if (heightScale > 0.0f) {
            heightScale -= 0.05f;
        } else {
            heightScale = 0.0f;
        }
    }

    void ObjWallRenderer::upScale() {
        if (heightScale < 1.0f) {
            heightScale += 0.05f;
        } else {
            heightScale = 1.0f;
        }
    }

} // Renderer