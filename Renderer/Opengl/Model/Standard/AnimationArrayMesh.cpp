#include "AnimationArrayMesh.h"

namespace Model {
    AnimationArrayMesh::AnimationArrayMesh(const shared_ptr<AnimationPlayer> &model,
                                           const shared_ptr<ShaderManager> &baseShader)
        : StandardMesh(baseShader), baseShader(baseShader) {
        setAnimationPlayer(model);
    }

    void AnimationArrayMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform, const bool shadows) const {
        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                parentTransform,
                shadows
            );
        } else {
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setBool("useMaterial", true);
            baseShader->setMat4("model", parentTransform);
        }

        renderMesh(parentTransform);

        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->unbind();
        }
    }

    void AnimationArrayMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                                             float dt, const glm::mat4 &parentTransform) const {
        const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material);
        if (standardMaterial && standardMaterial->isShadowEnabled()) {
            standardMaterial.get()->bindShadow(parentTransform);

            renderMesh(parentTransform * glm::mat4(1.0f));

            standardMaterial.get()->unbind();
        }
    }

    void AnimationArrayMesh::stop(const string &name) const {
        animationPlayer->stop(name);
    }

    void AnimationArrayMesh::play(const string &name) const {
        animationPlayer->start(name);
    }

    void AnimationArrayMesh::renderMesh(const glm::mat4 &parentTransform) const {
        const auto metadata = animationPlayer->play("KostraAction");

        // TODO: udelat to jeste ne jen s baseShaderem, ale i materialem

        for (int i = 0; i < metadata->bone_transform.size(); ++i) {
            baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", metadata->bone_transform[i]);
        }
        for (const auto &animMesh: animationPlayer->getMeshes()) {
            if (animMesh->getName() ==
                metadata->current_animation->nodes[0]->bone->meshName) {
                if (!animMesh->isHasBones()) {
                    baseShader->setBool("useBones", false);
                    baseShader->setMat4(
                        "model", parentTransform * animMesh->getGlobalTransformation());
                } else {
                    baseShader->setBool("useBones", true);
                    baseShader->setMat4("model", parentTransform);
                }
                animMesh->bind();
                glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()),
                               GL_UNSIGNED_INT,
                               nullptr);
            }
        }

        for (const auto& animMesh: animationPlayer->getNoBonesMeshes()) {
            baseShader->setBool("useBones", false);
            baseShader->setMat4("model", parentTransform * animMesh->getGlobalTransformation());
            animMesh->bind();
            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Model
