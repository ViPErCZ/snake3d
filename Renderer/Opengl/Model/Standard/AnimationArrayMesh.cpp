#include "AnimationArrayMesh.h"

namespace Model {
    AnimationArrayMesh::AnimationArrayMesh(const shared_ptr<AnimationModel> &model,
                                           const shared_ptr<ShaderManager> &baseShader) : StandardMesh(
            std::shared_ptr<BaseItem>(model->getBaseItem()), baseShader,
            0, 0), model(model), baseShader(baseShader) {
    }

    void AnimationArrayMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const {
        if (item->isVisible()) {
            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial.get()->bind(
                    camera->getPosition(),
                    camera->getViewMatrix(),
                    projection,
                    getBaseItem()->getModelMatrix()
                );
            } else {
                baseShader->setMat4("view", camera->getViewMatrix());
                baseShader->setMat4("projection", projection);
                baseShader->setVec3("viewPos", camera->getPosition());
                baseShader->setBool("useMaterial", true);
            }

            renderMesh();

            if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
                standardMaterial.get()->unbind();
            }
        }
    }

    void AnimationArrayMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                                             float dt) const {
        if (item->isVisible()) {
            const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material);
            if (standardMaterial && standardMaterial->isShadowEnabled()) {
                standardMaterial.get()->bindShadow(getBaseItem()->getModelMatrix());

                renderMesh();

                standardMaterial.get()->unbind();
            }
        }
    }

    void AnimationArrayMesh::renderMesh() const {
        auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                        [&](const auto &anim) {
                                            //return "KostraAction" == anim.name;
                                            return "Armature|Take 001|BaseLayer2" == anim.name;
                                        });

        if (found == model->getAnimations().end()) {
            found = model->getAnimations().begin();
        }

        if (found != model->getAnimations().end()) {
            const auto animation = &(*found);
            if (animationPlayer.get()) {
                model->updateAnimation(animation);
            }
            for (int i = 0; i < model->getMetadata(animation)->bone_transform.size(); ++i) {
                baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]",
                                    model->getMetadata(animation)->bone_transform[i]);
            }
            for (const auto animMesh: model->getMeshes()) {
                if (animMesh->getName() ==
                    animation->nodes[0].bone.meshName) {
                    if (!animMesh->isHasBones()) {
                        baseShader->setBool("useBones", false);
                        baseShader->setMat4(
                            "model", this->item->getModelMatrix() * animMesh->getGlobalTransformation());
                    } else {
                        baseShader->setBool("useBones", true);
                        baseShader->setMat4("model", this->item->getModelMatrix());
                    }
                    animMesh->bind();
                    glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()),
                                   GL_UNSIGNED_INT,
                                   nullptr);
                }
            }
        }

        for (const auto animMesh: model->getNoBonesMeshes()) {
            baseShader->setBool("useBones", false);
            baseShader->setMat4("model", this->item->getModelMatrix() * animMesh->getGlobalTransformation());
            animMesh->bind();
            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Model
