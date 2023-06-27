#include "AnimRenderer.h"
#include "Model/Utils/Tree.h"

namespace Renderer {

    AnimRenderer::AnimRenderer(const AnimationModel *sharedPtr, Camera *camera, const glm::mat4 &projection,
                               ResourceManager *resManager) {
        model = sharedPtr;
        resourceManager = resManager;
        this->camera = camera;
        this->projection = projection;
        baseShader = resourceManager->getShader("basicShader");
        bone_transform.resize(model->getBones().size(), glm::mat4(1.0f));

        const auto found = std::find_if(model->getAnimations().begin(), model->getAnimations().end(),
                                        [&](const auto &anim) {
                                            return "Armature|Take 001|BaseLayer" == anim.name; // Take 001 | Armature|Take 001|BaseLayer | KostraAction
                                        });
        if (found != model->getAnimations().end()) {
            animation = &(*found);
            animation_duration = std::chrono::seconds(0);
            last_time = std::chrono::time_point<std::chrono::steady_clock>();
        }
    }

    void AnimRenderer::render() {
        baseShader->use();
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setVec3("viewPos", camera->getPosition());
        baseShader->setBool("useMaterial", false);

        glm::mat4 modelTrans = glm::mat4(1.0f);
        modelTrans = glm::translate(modelTrans, {0.0, 0.0, 0.0});
//        modelTrans = glm::scale(modelTrans, {10.041666667f, 10.041666667f, 10.041666667f});
        modelTrans = glm::scale(modelTrans, {0.41666667f, 0.41666667f, 0.41666667f});
        int counter = 0;

        for (int i = 0; i < bone_transform.size(); ++i) {
            baseShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", bone_transform[i]);
        }

        resourceManager->getTexture("Skeleton_Body.png")->bind();

        for (auto item: model->getMeshes()) {
            if (animation) {
                if (!item->isHasBones()) {
                    baseShader->setBool("useBones", false);
                    baseShader->setMat4("model", modelTrans * item->getGlobalTransformation());
                } else {
                    baseShader->setBool("useBones", true);
                    baseShader->setMat4("model", modelTrans);
                }
            } else {
                baseShader->setMat4("model", modelTrans);
            }
            item->bind();
            glDrawElements(GL_TRIANGLES, (int) item->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            counter++;
        }
    }

    void AnimRenderer::renderShadowMap() {
    }

    void AnimRenderer::beforeRender() {
        if (animation) {
            const Animation &anim = *animation;
            const auto current_time = std::chrono::steady_clock::now();
            if (last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
                last_time = current_time;
            }
            const auto delta_time = current_time - last_time;
            animation_duration += delta_time;
            last_time = current_time;
            const auto animation_time = glm::mod(animation_duration.count() * anim.tps, anim.duration);

            std::function<void(const Tree<uint32_t> &, const glm::mat4 &)> node_traversal;
            node_traversal = [&](const Tree<uint32_t> &node, const glm::mat4 &parent_mat) {
                auto anim_node = findAnimationNode(model->getBones()[*node]);
                auto local_transform = !model->getBones()[*node].isFake() ? model->getBones()[*node].node_transform
                                                                          : glm::mat4(1.f);

                if (anim_node) {
                    glm::vec3 scale = anim_node->scalingLerp(animation_time);
                    glm::vec3 position = anim_node->positionLerp(animation_time);
                    auto rotation = anim_node->rotationLerp(animation_time);

                    auto translate = glm::translate(glm::mat4(1.f), position);
                    auto rotate = glm::mat4_cast(rotation);
                    auto scale_mat = glm::scale(glm::mat4(1.f), scale);

                    local_transform = translate * rotate * scale_mat;
                }

                auto transform = parent_mat * local_transform;

                if (anim_node) {
                    //bone_transform[*node] = local_transform;
//                } else {
                    bone_transform[*node] = parent_mat * local_transform *
                                            (model->getBones()[*node]).offset_matrix;
                } else {
                    bone_transform[*node] = local_transform;
                }

                for (const auto &n: node) {
                    node_traversal(n, transform);
                }
            };

            try {
                node_traversal(model->getSkeleton(), glm::mat4(1.f));
            } catch (const std::exception &e) {
                throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
            }
        }
    }

    void AnimRenderer::afterRender() {

    }

    const AnimationNode *AnimRenderer::findAnimationNode(const Bone &bone) const noexcept {
        if (animation) {
            for (auto &node: animation->nodes) {
                if (&node.bone == &bone) {
                    return &node;
                }
            }
        }

        return nullptr;
    }
} // Renderer