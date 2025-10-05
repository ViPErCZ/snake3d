#include "AnimRenderer.h"

namespace Renderer {

    AnimRenderer::AnimRenderer(sSNAKE_TILE* tile, AnimationModel *sharedPtr, Camera *camera, const glm::mat4 &projection,
                               ResourceManager *resManager): show(true) {
        model = sharedPtr;
        resourceManager = resManager;
        this->camera = camera;
        this->projection = projection;
        this->tile = tile;
        shader = resourceManager->getShader("normalShader").get();
        shadowShader = resourceManager->getShader("shadowDepthShader").get();
    }

    AnimRenderer::~AnimRenderer() {
        animationPlay.clear();
    }

    void AnimRenderer::render(float dt) {
        if (show) {
            shader->use();
            shader->setMat4("view", camera->getViewMatrix());
            shader->setMat4("projection", projection);
            shader->setVec3("viewPos", camera->getPosition());
            shader->setBool("useMaterial", true);
            const glm::vec3 lightPos(
                model->getBaseItem()->getPosition().x,
                model->getBaseItem()->getPosition().y,
                model->getBaseItem()->getPosition().z + glfwGetTime()
            );
            // shader->setVec3("lightPos", lightPos);
            shader->setBool("useMaterial", true);
            // directional light
            shader->setVec3("materialDirLight.direction", lightPos.x, lightPos.y, lightPos.z);
            shader->setVec3("materialDirLight.diffuse", 0.8f, 0.8f, 0.8f);
            shader->setVec3("materialDirLight.specular", 0.5f, 0.5f, 0.5f);
            // point light 1
            shader->setFloat("material.shininess", 32.0f);

            renderScene(shader);

            shader->setBool("useBones", false);
            shader->setBool("useMaterial", false);
        }
    }

    void AnimRenderer::renderScene(const ShaderManager *shader) {
        const glm::vec4 *rotate = model->getBaseItem()->getRotate();

        switch (tile->direction) {
            case ItemsDto::RIGHT:
                model->getBaseItem()->setRotate({1, 0, 0, 90}, {0, 1, 0, 0}, rotate[2]);
                break;
            case ItemsDto::LEFT:
                model->getBaseItem()->setRotate({1, 0, 0, 90}, {0, 1, 0, 180}, rotate[2]);
                break;
            case ItemsDto::UP:
                model->getBaseItem()->setRotate({1, 0, 0, 90}, {0, 1, 0, 90}, rotate[2]);
                break;
            case ItemsDto::DOWN:
                model->getBaseItem()->setRotate({1, 0, 0, 90}, {0, 1, 0, -90}, rotate[2]);
                break;
            default:
                model->getBaseItem()->setRotate({1, 0, 0, 90}, {0, 1, 0, 0}, rotate[2]);
                break;
        }

        for (auto animName: animationPlay) {
            const auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                            [&](const auto &anim) {
                                                return animName == anim.name;
                                            });
            if (found != model->getAnimations().end()) {
                const auto animation = &(*found);
                model->updateAnimation(animation);
                for (int i = 0; i < model->getMetadata(animation)->bone_transform.size(); ++i) {
                    shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]",
                                        model->getMetadata(animation)->bone_transform[i]);
                }
                for (const auto item: model->getMeshes()) {
                    if (item->getName() ==
                        animation->nodes[0].bone.meshName) {
                        if (!item->isHasBones()) {
                            shader->setBool("useBones", false);
                            shader->setMat4("model", model->getBaseItem()->getModelMatrix() * item->getGlobalTransformation());
                        } else {
                            shader->setBool("useBones", true);
                            shader->setMat4("model", model->getBaseItem()->getModelMatrix());
                        }
                        item->bind();
                        glDrawElements(GL_TRIANGLES, static_cast<int>(item->getIndices().size()), GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
        }

        for (const auto item: model->getNoBonesMeshes()) {
            shader->setBool("useBones", false);
            shader->setMat4("model", model->getBaseItem()->getModelMatrix() * item->getGlobalTransformation());
            item->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(item->getIndices().size()), GL_UNSIGNED_INT, nullptr);
        }
    }

    shared_ptr<Mesh> AnimRenderer::getMesh() {
        return nullptr;
    }

    void AnimRenderer::renderShadowMap() {
        shadowShader->use();
        renderScene(shadowShader);
    }

    void AnimRenderer::beforeRender() {
        glEnable(GL_DEPTH_TEST);
    }

    void AnimRenderer::afterRender() {
        glDisable(GL_DEPTH_TEST);
    }

    void AnimRenderer::addPlay(const string& name) {
        animationPlay.push_back(name);
    }

    void AnimRenderer::setShow(const bool show) {
        AnimRenderer::show = show;
    }

    void AnimRenderer::setAcceleration(const float acceleration) const {
        model->setAcceleration(acceleration);
    }
} // Renderer