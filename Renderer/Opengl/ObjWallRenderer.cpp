#include "ObjWallRenderer.h"

namespace Renderer {
    ObjWallRenderer::ObjWallRenderer(const shared_ptr<Snake> &snake, const shared_ptr<ObjWall> &item, Camera* camera, const glm::mat4 &proj, ResourceManager* resManager)
        : snake(snake), wall(item), camera(camera), projection(proj), resourceManager(resManager), parallax(false) {
        //mesh = resourceManager->getModel("cube")->getMesh();
        shader = resourceManager->getShader("normalShader").get();
        texture1 = resourceManager->getTexture("brickwork-texture.jpg").get();
        texture2 = resourceManager->getTexture("brickwork_normal-map.jpg").get();
        texture3 = resourceManager->getTexture("brickwork-bump-map.jpg").get();
        texture4 = resourceManager->getTexture("bricks2_disp.jpg").get();
    }

    ObjWallRenderer::~ObjWallRenderer() {
        // delete wall;
    }

    void ObjWallRenderer::render3D(float dt) {
        shader->use();

        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", this->projection);
        shader->setInt("diffuseMap", 0);
        shader->setInt("normalMap", 1);
        shader->setInt("specularMap", 2);
        shader->setInt("depthMap", 2);
        shader->setBool("parallaxEnable", parallax);
        shader->setFloat("alpha", 0.2);
        shader->setBool("fogEnable", fog);

        // lighting info
        // -------------
        glm::vec3 lightPos(camera->getPosition().x - 26, camera->getPosition().y - 26, 36.3f);

        const float fadeSpeed = 4.0f * dt;

        for (const auto item: wall->getItems()) {
            glLoadIdentity();

            bool isOccluding = false;

            for (auto Iter = snake->getItems().begin(); Iter < snake->getItems().end(); ++Iter) {
                glm::vec3 snakePos = (*Iter)->tile->getPosition();
                glm::vec3 cameraPos = snakePos;
                cameraPos.y -= 2;
                cameraPos.x -= 1;
                glm::vec3 rayDir = glm::normalize(snakePos - cameraPos);
                float rayLen = glm::distance(snakePos, cameraPos);

                // Zde si spočti AABB pro objekt (např. z pozice a velikosti)
                glm::vec3 itemMin = item->getPosition() - glm::vec3(1.0f, 1.0f, 1.0f); // bounding box offset
                glm::vec3 itemMax = item->getPosition() + glm::vec3(1.0f, 1.0f, 1.0f); // bounding box offset

                if (rayIntersectsAABB(cameraPos, rayDir, itemMin, itemMax, rayLen)) {
                    // item je mezi kamerou a hadem → zprůhlednit
                    isOccluding = true;
                    break;
                }
            }

            const float targetAlpha = isOccluding ? 0.1f : 1.0f;
            if (item->getCurrentAlpha() == 1.0f) {
                item->setCurrentAlpha(targetAlpha);
            } else {
                item->setCurrentAlpha(glm::mix(item->getCurrentAlpha(), targetAlpha, fadeSpeed));
            }

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
            shader->setFloat("alpha", item->getCurrentAlpha());

            // directional light
            shader->setVec3("dirLight.direction", lightPos.x, lightPos.y, lightPos.z);
            shader->setVec3("dirLight.ambient", 0.005f, 0.005f, 0.05f);
            shader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
            // point light 1
            shader->setFloat("material.shininess", 32.0f);
            shader->setInt("material.diffuse", 0);
            shader->setInt("material.specular", 1);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
        }

        glEnable(GL_TEXTURE0);
    }

    void ObjWallRenderer::beforeRender() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void ObjWallRenderer::afterRender() {
        glDisable(GL_BLEND);

        texture1->unbind(0);
        texture2->unbind(1);
        texture3->unbind(2);
    }

    void ObjWallRenderer::renderShadowMap() {

    }

    void ObjWallRenderer::toggleParallax() {
        parallax = !parallax;

        if (parallax) {
            texture1 = resourceManager->getTexture("bricks2.jpg").get();
            texture2 = resourceManager->getTexture("bricks2_normal.jpg").get();
            texture4 = resourceManager->getTexture("bricks2_disp.jpg").get();
        } else {
            texture1 = resourceManager->getTexture("brickwork-texture.jpg").get();
            texture2 = resourceManager->getTexture("brickwork_normal-map.jpg").get();
            texture3 = resourceManager->getTexture("brickwork-bump-map.jpg").get();
        }
    }

    bool ObjWallRenderer::rayIntersectsAABB(
        const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, const float maxDistance) {
        float tMin = 0.0f;
        float tMax = maxDistance;

        for (int i = 0; i < 3; ++i) {
            if (std::abs(rayDir[i]) < 1e-8) {
                if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                    return false; // Ray parallel to slab
            } else {
                float ood = 1.0f / rayDir[i];
                float t1 = (boxMin[i] - rayOrigin[i]) * ood;
                float t2 = (boxMax[i] - rayOrigin[i]) * ood;
                if (t1 > t2) std::swap(t1, t2);
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                if (tMin > tMax)
                    return false;
            }
        }
        return true;
    }

} // Renderer