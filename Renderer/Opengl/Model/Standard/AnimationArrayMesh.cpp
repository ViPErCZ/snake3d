#include "AnimationArrayMesh.h"

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "../../Material/MaterialInstance.h"
#include "../../Material/RenderContext.h"
#include "../../Material/ShaderMaterial.h"
#include "../../RenderStats.h"

namespace Model {
    AnimationArrayMesh::AnimationArrayMesh(const shared_ptr<AnimationPlayer> &model,
                                           const shared_ptr<ShaderProgram> &baseShader, const string &animationName)
        : StandardMesh(baseShader), baseShader(baseShader) {
        setAnimationPlayer(model);
        animation = animationName;
    }

    void AnimationArrayMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform, const bool shadows) const {
        if (const auto materialInstance = dynamic_pointer_cast<const MaterialInstance>(material)) {
            const RenderContext ctx{
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                parentTransform,
                static_cast<float>(glfwGetTime()),
                shadows
            };
            materialInstance->bind(ctx);
        } else if (const auto shaderMaterial = dynamic_pointer_cast<const ShaderMaterial>(material)) {
            shaderMaterial->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                parentTransform,
                shadows
            );
        } else {
            // Fallback branch: no MaterialInstance / ShaderMaterial.
            // D1.1b: view/projection/viewPos arrive via FrameData UBO -
            // bound by RenderManager once per pass.
            baseShader->use();
            baseShader->setMat4("model", parentTransform);
        }

        renderMesh(parentTransform);

        if (const auto materialInstance = dynamic_pointer_cast<const MaterialInstance>(material)) {
            materialInstance->unbind();
        } else if (const auto shaderMaterial = dynamic_pointer_cast<const ShaderMaterial>(material)) {
            shaderMaterial->unbind();
        }
    }

    void AnimationArrayMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                                             float dt, const glm::mat4 &parentTransform) const {
        if (const auto materialInstance = std::dynamic_pointer_cast<const MaterialInstance>(material)) {
            if (materialInstance->bindShadow(parentTransform)) {
                renderMesh(parentTransform * glm::mat4(1.0f), false);
            }
            return;
        }
        if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            if (shaderMaterial->isShadowEnabled() && shaderMaterial->getShadowDepthShader()) {
                shaderMaterial->bindShadow(parentTransform);
                renderMesh(parentTransform * glm::mat4(1.0f), false);
                shaderMaterial->unbind();
            }
        }
    }

    void AnimationArrayMesh::renderMesh(const glm::mat4 &parentTransform, const bool animPlay) const {
        shared_ptr<AnimationMeta> metadata;
        try {
            metadata = animPlay ? animationPlayer->play(animation) : animationPlayer->getMetadata(animation);
        } catch (const std::invalid_argument &) {
            return;
        }

        // Resolve once: pickneme aktivní program (main vs shadow) ze
        // current material a pak ho přímo poke-ujeme. Tady už nezáleží na
        // konkrétním type kromě toho jak najít shader.
        std::shared_ptr<Manager::ShaderProgram> activeProgram;
        if (const auto materialInstance = std::dynamic_pointer_cast<const MaterialInstance>(material)) {
            activeProgram = animPlay
                ? materialInstance->getProgram()
                : materialInstance->getShadowProgram();
        } else if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            activeProgram = animPlay
                ? shaderMaterial->getShader()
                : shaderMaterial->getShadowDepthShader();
        }

        for (int i = 0; i < metadata->bone_transform.size(); ++i) {
            if (activeProgram) {
                activeProgram->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", metadata->bone_transform[i]);
            } else {
                baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", metadata->bone_transform[i]);
            }
        }
        for (const auto &animMesh: animationPlayer->getMeshes()) {
            if (animMesh->getName() ==
                metadata->current_animation->nodes[0]->bone->meshName) {
                glm::mat4 finalTransform = animMesh->isHasBones() ? parentTransform : parentTransform * animMesh->getGlobalTransformation();
                if (activeProgram) {
                    activeProgram->setBool("useBones", animMesh->isHasBones());
                    activeProgram->setMat4("model", finalTransform);
                } else {
                    baseShader->setBool("useBones", animMesh->isHasBones());
                    baseShader->setMat4("model", finalTransform);
                }

                animMesh->bind();
                Renderer::RenderStats::countDraw();
                glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()),
                               GL_UNSIGNED_INT,
                               nullptr);
            }
        }

        for (const auto& animMesh: animationPlayer->getNoBonesMeshes()) {
            const glm::mat4 worldTransform = parentTransform * animMesh->getGlobalTransformation();
            if (activeProgram) {
                activeProgram->setBool("useBones", false);
                activeProgram->setMat4("model", worldTransform);
            } else {
                baseShader->setBool("useBones", false);
                baseShader->setMat4("model", worldTransform);
            }
            animMesh->bind();
            Renderer::RenderStats::countDraw();
            glDrawElements(GL_TRIANGLES, static_cast<int>(animMesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
} // Model
