#include "AnimationArrayMesh.h"

#include <stdexcept>

namespace Model {
    AnimationArrayMesh::AnimationArrayMesh(const shared_ptr<AnimationPlayer> &model,
                                           const shared_ptr<ShaderManager> &baseShader, const string &animationName)
        : StandardMesh(baseShader), baseShader(baseShader) {
        setAnimationPlayer(model);
        animation = animationName;
    }

    void AnimationArrayMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform, const bool shadows) const {
        if (const auto standardMaterial = dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                parentTransform,
                shadows
            );
        } else {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setBool("useMaterial", true);
            baseShader->setMat4("model", parentTransform);
        }

        renderMesh(parentTransform);

        if (const auto standardMaterial = dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->unbind();
        }
    }

    void AnimationArrayMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                                             float dt, const glm::mat4 &parentTransform) const {
        const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material);
        if (standardMaterial && standardMaterial->isShadowEnabled()) {
            standardMaterial.get()->bindShadow(parentTransform);

            renderMesh(parentTransform * glm::mat4(1.0f), false);

            standardMaterial.get()->unbind();
        }
    }

    void AnimationArrayMesh::renderMesh(const glm::mat4 &parentTransform, const bool animPlay) const {
        shared_ptr<AnimationMeta> metadata;
        try {
            metadata = animPlay ? animationPlayer->play(animation) : animationPlayer->getMetadata(animation);
        } catch (const std::invalid_argument &) {
            return;
        }

        for (int i = 0; i < metadata->bone_transform.size(); ++i) {
            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial->bindBonesMatrices(i, metadata->bone_transform[i]);
            } else {
                baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", metadata->bone_transform[i]);
            }
        }
        for (const auto &animMesh: animationPlayer->getMeshes()) {
            if (animMesh->getName() ==
                metadata->current_animation->nodes[0]->bone->meshName) {
                glm::mat4 finalTransform = animMesh->isHasBones() ? parentTransform : parentTransform * animMesh->getGlobalTransformation();
                if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                    standardMaterial->bindUseBones(animMesh->isHasBones());
                    if (animPlay) {
                        standardMaterial->bindModel(finalTransform);
                    } else {
                        standardMaterial->bindShadowModel(finalTransform);
                    }
                } else {
                    baseShader->setBool("useBones", animMesh->isHasBones());
                    baseShader->setMat4(
                        "model", finalTransform);
                }

                animMesh->bind();
                glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()),
                               GL_UNSIGNED_INT,
                               nullptr);
            }
        }

        for (const auto& animMesh: animationPlayer->getNoBonesMeshes()) {
            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial->bindUseBones(false);
                if (animPlay) {
                    standardMaterial->bindModel(parentTransform * animMesh->getGlobalTransformation());
                } else {
                    standardMaterial->bindShadowModel(parentTransform * animMesh->getGlobalTransformation());
                }
            } else {
                baseShader->setBool("useBones", false);
                baseShader->setMat4("model", parentTransform * animMesh->getGlobalTransformation());
            }
            animMesh->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Model
