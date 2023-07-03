#include "AnimRenderer.h"
#include "Model/Utils/Tree.h"

namespace Renderer {

    AnimRenderer::AnimRenderer(const AnimationModel *sharedPtr, Camera *camera, const glm::mat4 &projection,
                               ResourceManager *resManager) {
        model = sharedPtr;
        resourceManager = resManager;
        this->camera = camera;
        this->projection = projection;
        baseShader = resourceManager->getShader("normalShader");
    }

    void AnimRenderer::render() {
        baseShader->use();
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setVec3("viewPos", camera->getPosition());
        baseShader->setBool("useMaterial", true);
        baseShader->setVec3("lightPos", glm::vec3(-20.0, 26.0, 60.0));

        glm::mat4 modelTrans = glm::mat4(1.0f);
        modelTrans = glm::translate(modelTrans, {0.0, 0.0, 0.0});
//        modelTrans = glm::scale(modelTrans, {10.041666667f, 10.041666667f, 10.041666667f});
        modelTrans = glm::scale(modelTrans, {0.41666667f, 0.41666667f, 0.41666667f});

//        resourceManager->getTexture("Skeleton_Body.png")->bind();

        for (auto animName: animationPlay) {
            const auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                            [&](const auto &anim) {
                return animName == anim.name; // Take 001 | Armature|Take 001|BaseLayer | KostraAction
            });
            if (found != model->getAnimations().end()) {
                auto animation = &(*found);
                model->updateAnimation(animation);
                for (int i = 0; i < model->getMetadata(animation)->bone_transform.size(); ++i) {
                    baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", model->getMetadata(animation)->bone_transform[i]);
                }
                for (auto item: model->getMeshes()) {
                    if (item->getName() ==
                        animation->nodes[0].bone.meshName) { // Mesh patrici pod animaci Kostra2Action.002, jmena Right_Pupil1
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

    void AnimRenderer::renderShadowMap() {
    }

    void AnimRenderer::beforeRender() {
    }

    void AnimRenderer::afterRender() {

    }

    void AnimRenderer::addPlay(const string& name) {
        animationPlay.push_back(name);
    }
} // Renderer