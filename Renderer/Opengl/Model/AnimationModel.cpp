#include "AnimationModel.h"
#include <functional>

namespace Model {
    AnimationModel::AnimationModel(BaseItem* item,
                                   const vector<Mesh *> &meshes,
                                   decltype(animations)&& _animations,
                                   decltype(bones)&& bones,
                                   decltype(skeleton)&& skeleton,
                                   decltype(bones_map)&& bones_map,
                                   const glm::mat4& _global_matrix) :
            baseItem(item), meshes(meshes), animations(std::move(_animations)), bones(std::move(bones)),
            skeleton(skeleton), bones_map(bones_map), global_inverse{_global_matrix}, globalPause(false) {

        this->acceleration = 1.0;
        std::erase_if(
            this->meshes,
            [this](Mesh *p) {
                if (!p->isHasBones()) {
                    noBonesMeshes.push_back(p);
                    return true;
                }
                return false;
            }
        );

        for (const auto& anim: animations) {
            auto meta = new AnimationMeta;
            meta->name = anim.name;
            meta->animation_duration = std::chrono::seconds(0);
            meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
            meta->bone_transform.resize(this->bones.size(), glm::mat4(1.0f));
            meta->pause = globalPause;
            metadata.emplace(anim.name, meta);
        }
    }

    AnimationModel::~AnimationModel() {
        animations.clear();
        bones.clear();
        bones_map.clear();

        for (auto &[fst, snd] : metadata) {
            delete snd;
        }

        metadata.clear();

        for (const auto mesh : meshes) {
            delete mesh;
        }

        meshes.clear();

        for (const auto mesh : noBonesMeshes) {
            delete mesh;
        }

        noBonesMeshes.clear();
    }

    const vector<Mesh *> &AnimationModel::getMeshes() const {
        return meshes;
    }

    const vector<Animation> &AnimationModel::getAnimations() const {
        return animations;
    }

    const vector<Bone> &AnimationModel::getBones() const {
        return bones;
    }

    const Tree<uint32_t> &AnimationModel::getSkeleton() const {
        return skeleton;
    }

    const glm::mat4 &AnimationModel::getGlobalInverse() const {
        return global_inverse;
    }

    const unordered_map<std::string, uint32_t> &AnimationModel::getBonesMap() const {
        return bones_map;
    }

    const vector<Mesh *> &AnimationModel::getNoBonesMeshes() const {
        return noBonesMeshes;
    }

    const AnimationNode *
    AnimationModel::findAnimationNode(const Animation *animation, const Bone &bone) noexcept {
        for (auto &node: animation->nodes) {
            if (&node.bone == &bone) {
                return &node;
            }
        }

        return nullptr;
    }

    void AnimationModel::updateAnimation(const Animation *animation) const {
        const auto meta = metadata.at(animation->name);
        if (meta && !meta->pause) {
            const Animation &anim = *animation;
            const auto current_time = std::chrono::steady_clock::now();
            if (meta->last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
                meta->last_time = current_time;
            }
            const auto delta_time = current_time - meta->last_time;
            meta->animation_duration += delta_time * this->acceleration;
            meta->last_time = current_time;

            // TOTO ZAJISTUJE, ze se animace prehrava cyklicky
            // pokud by animace mela byt jen jednou, tak se to muze odstranit
            // const auto animation_time = glm::mod(meta->animation_duration.count() * anim.tps, anim.duration);
            const auto animation_time = meta->animation_duration.count() * anim.tps;
            if (animation_time >= anim.duration) {
                return;
            }

            std::function<void(const Tree<uint32_t> &, const glm::mat4 &)> node_traversal;
            node_traversal = [&](const Tree<uint32_t> &node, const glm::mat4 &parent_mat) {
                const auto anim_node = findAnimationNode(animation, getBones()[*node]);
                auto local_transform = !getBones()[*node].isFake() ? getBones()[*node].node_transform
                                                                          : glm::mat4(1.f);

                if (anim_node) {
                    const glm::vec3 scale = anim_node->scalingLerp(animation_time);
                    const glm::vec3 position = anim_node->positionLerp(animation_time);
                    const auto rotation = anim_node->rotationLerp(animation_time);

                    const auto translate = glm::translate(glm::mat4(1.f), position);
                    const auto rotate = glm::mat4_cast(rotation);
                    const auto scale_mat = glm::scale(glm::mat4(1.f), scale);

                    local_transform = translate * rotate * scale_mat;
                }

                const auto transform = parent_mat * local_transform;

                if (anim_node) {
                    meta->bone_transform[*node] = parent_mat * local_transform *
                                            (getBones()[*node]).offset_matrix;
                } else {
                    meta->bone_transform[*node] = local_transform;
                }

                for (const auto &n: node) {
                    node_traversal(n, transform);
                }
            };

            try {
                node_traversal(getSkeleton(), glm::mat4(1.f));
            } catch (const std::exception &e) {
                throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
            }
        } else if (meta) {
            meta->last_time = std::chrono::steady_clock::now();
        }
    }

    AnimationMeta *AnimationModel::getMetadata(const Animation *animation) const {
        return metadata.at(animation->name);
    }

    BaseItem *AnimationModel::getBaseItem() const {
        return baseItem;
    }

    void AnimationModel::setBaseItem(BaseItem *baseItem) {
        AnimationModel::baseItem = baseItem;
    }

    void AnimationModel::setGlobalPause(const bool globalPause) {
        AnimationModel::globalPause = globalPause;
        for (const auto &[key, meta]: metadata) {
            meta->pause = globalPause;
        }
    }

    void AnimationModel::setAcceleration(const float acceleration) {
        this->acceleration = acceleration;
    }

} // Model