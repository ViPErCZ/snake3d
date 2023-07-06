#include "AnimRenderer.h"
#include "Model/Utils/Tree.h"

namespace Renderer {

    AnimRenderer::AnimRenderer(sSNAKE_TILE* tile, const AnimationModel *sharedPtr, Camera *camera, const glm::mat4 &projection,
                               ResourceManager *resManager): show(true) {
        model = sharedPtr;
        resourceManager = resManager;
        this->camera = camera;
        this->projection = projection;
        this->tile = tile;
        baseShader = resourceManager->getShader("normalShader");
    }

    AnimRenderer::~AnimRenderer() {
        animationPlay.clear();
    }

    void AnimRenderer::render() {
        if (show) {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setBool("useMaterial", true);
            glm::vec3 lightPos(model->getBaseItem()->getPosition().x, model->getBaseItem()->getPosition().y + 6, -5.3f);
            baseShader->setVec3("lightPos", glm::vec3(-20.0, 26.0, 60.0));
            baseShader->setVec3("lightPos", lightPos);

            glm::mat4 modelTrans = glm::mat4(1.0f);
            glm::vec3 position = model->getBaseItem()->getPosition();
            modelTrans = glm::translate(modelTrans, {0.0, 0.0, 0.0});
            modelTrans = glm::scale(modelTrans, {0.041667f, 0.041667f, 0.041667f});
            modelTrans = glm::translate(modelTrans, position);

            const glm::vec4 *rotate = model->getBaseItem()->getRotate();

            switch (tile->direction) {
                case ItemsDto::RIGHT:
                    model->getBaseItem()->setRotate({90, 1, 0, 0}, {0, 0, 1, 0}, rotate[2]);
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[0].x), {1.0, 0.0, 0.0f});
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[1].x), {0.0, 1.0, 0.0f});
                    break;
                case ItemsDto::LEFT:
                    model->getBaseItem()->setRotate({90, 1, 0, 0}, {180, 0, 1, 0}, rotate[2]);
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[0].x), {1.0, 0.0, 0.0f});
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[1].x), {0.0, 1.0, 0.0f});
                    break;
                case ItemsDto::UP:
                    model->getBaseItem()->setRotate({90, 1, 0, 0}, {90, 0, 1, 0}, rotate[2]);
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[0].x), {1.0, 0.0, 0.0f});
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[1].x), {0.0, 1.0, 0.0f});
                    break;
                case ItemsDto::DOWN:
                    model->getBaseItem()->setRotate({90, 1, 0, 0}, {-90, 0, 1, 0}, rotate[2]);
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[0].x), {1.0, 0.0, 0.0f});
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[1].x), {0.0, 1.0, 0.0f});
                    break;
                default:
                    model->getBaseItem()->setRotate({90, 1, 0, 0}, {0, 0, 1, 0}, rotate[2]);
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[0].x), {1.0, 0.0, 0.0f});
                    modelTrans = glm::rotate(modelTrans, glm::radians(rotate[1].x), {0.0, 1.0, 0.0f});
                    break;
            }

            for (auto animName: animationPlay) {
                const auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                                [&](const auto &anim) {
                                                    return animName == anim.name;
                                                });
                if (found != model->getAnimations().end()) {
                    auto animation = &(*found);
                    model->updateAnimation(animation);
                    for (int i = 0; i < model->getMetadata(animation)->bone_transform.size(); ++i) {
                        baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]",
                                            model->getMetadata(animation)->bone_transform[i]);
                    }
                    for (auto item: model->getMeshes()) {
                        if (item->getName() ==
                            animation->nodes[0].bone.meshName) {
                            if (!item->isHasBones()) {
                                baseShader->setBool("useBones", false);
                                baseShader->setMat4("model", modelTrans * item->getGlobalTransformation());
                            } else {
                                baseShader->setBool("useBones", true);
                                baseShader->setMat4("model", modelTrans);
                            }
                            item->bind();
                            glDrawElements(GL_TRIANGLES, (int) item->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                        }
                    }
                }
            }

            for (auto item: model->getNoBonesMeshes()) {
                baseShader->setBool("useBones", false);
                baseShader->setMat4("model", modelTrans * item->getGlobalTransformation());
                item->bind();
                glDrawElements(GL_TRIANGLES, (int) item->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            }

            baseShader->setBool("useBones", false);
            baseShader->setBool("useMaterial", false);
        }
    }

    void AnimRenderer::renderShadowMap() {
    }

    void AnimRenderer::beforeRender() {
    }

    void AnimRenderer::afterRender() {

    }

    void AnimRenderer::addPlay(const string& name) {
        animationPlay.push_back(name);
    }

    void AnimRenderer::setShow(bool show) {
        AnimRenderer::show = show;
    }

} // Renderer