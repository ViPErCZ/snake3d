#include "AnimationArrayMesh.h"

namespace Model {
    AnimationArrayMesh::AnimationArrayMesh(const shared_ptr<AnimationModel> &model,
                                           const shared_ptr<ShaderManager> &baseShader)
        : StandardMesh(baseShader), model(model), baseShader(baseShader) {
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

    void AnimationArrayMesh::stop(const bool stop) const {
        model->setGlobalPause(stop);
    }

    void AnimationArrayMesh::play(const string &animation) {
        model->setGlobalPause(false);
    }

    void AnimationArrayMesh::setAnimationPlayer(const shared_ptr<AnimationPlayer> &animationPlayer) {
        this->animationPlayer = animationPlayer;
    }

    void AnimationArrayMesh::renderMesh(const glm::mat4 &parentTransform) const {
        auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                        [&](const auto &anim) {
                                            return "KostraAction" == anim.name; // TODO: dynamic anim name
                                            // return "Armature|Take 001|BaseLayer2" == anim.name;
                                        });

        if (found == model->getAnimations().end()) {
            found = model->getAnimations().begin();
        }

        if (found != model->getAnimations().end()) {
            const auto animation = &(*found);
            // TODO: speed animace bude v anim playeru
            //if (animationPlayer.get()) { // TODO: toto povolit az budu mit vyse dynamicky nazev animace
                model->updateAnimation(animation);
            //}
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
        }

        for (const auto animMesh: model->getNoBonesMeshes()) {
            baseShader->setBool("useBones", false);
            baseShader->setMat4("model", parentTransform * animMesh->getGlobalTransformation());
            animMesh->bind();
            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Model
