#include "AnimationPlayer.h"

#include <chrono>
#include <functional>
#include <ranges>

namespace Animations {
    AnimationPlayer::AnimationPlayer(const string &name) {
        createAnimation(name);
    }

    AnimationPlayer::AnimationPlayer(const vector<shared_ptr<Mesh>> &meshes,
                                     const map<string, shared_ptr<Animation>> &animations, const vector<shared_ptr<Bone>> &bones,
                                     const Tree<uint32_t> &skeleton, const unordered_map<std::string, uint32_t> &bones_map,
                                     const glm::mat4 &global_matrix) :
        animations(animations), meshes(meshes), bones(bones), bones_map(bones_map), skeleton(skeleton),
        global_inverse(global_matrix) {

        std::erase_if(
            this->meshes,
            [this](const shared_ptr<Mesh> &p) {
                if (!p->isHasBones()) {
                    noBonesMeshes.push_back(p);
                    return true;
                }
                return false;
            }
        );

        for (const auto &key: animations | views::keys) {
            auto meta = new AnimationMeta;
            meta->name = key;
            meta->animation_duration = std::chrono::seconds(0);
            meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
            meta->bone_transform.resize(this->bones.size(), glm::mat4(1.0f));
            meta->pause = true;
            meta->alpha = 1.0f;
            meta->world_transform = glm::mat4(1.0f);
            metadata.emplace(key, meta);
        }
    }

    void AnimationPlayer::createAnimation(const string &name) {
        animations.emplace(name, make_shared<Animation>(name, 0, 25));
        const auto meta = make_shared<AnimationMeta>();
        meta->name = name;
        meta->animation_duration = std::chrono::seconds(0);
        meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
        meta->pause = false;
        meta->alpha = 1.0f;
        meta->world_transform = glm::mat4(1.0f);
        metadata.emplace(name, meta);
    }

    void AnimationPlayer::addAnimationNode(const string &name,
        const shared_ptr<AnimationNode> &animationNode, const int duration) {
        const auto anim = animations.find(name);
        if (anim == animations.end()) {
            throw std::invalid_argument("Animation not found (in addAnimationNode): " + name);
        }

        const auto meta = metadata.at(name);
        if (nullptr == meta) {
            throw std::invalid_argument("Animation metadata not found: " + name);
        }

        anim->second->duration = duration;
        anim->second->nodes.emplace_back(animationNode);
    }

    void AnimationPlayer::setAcceleration(const float acceleration) {
        this->acceleration = acceleration;
    }

    void AnimationPlayer::start(const string &name, const bool loop) {
        this->setRepeat(loop);
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found");
        }
        meta->pause = false;
        meta->alpha = 1.0f;
    }

    void AnimationPlayer::stop(const string &name) const {
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found");
        }
        meta->animation_duration = std::chrono::seconds(0);
        meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
        meta->pause = true;
        meta->alpha = 1.0f;
        meta->world_transform = glm::mat4(1.0f);
    }

    void AnimationPlayer::pause(const string &name) const {
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found");
        }
        meta->pause = true;
    }

    void AnimationPlayer::resume(const string &name) const {
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found");
        }
        meta->pause = false;
    }

    shared_ptr<AnimationMeta> AnimationPlayer::play(const string &name) {
        const auto anim = animations.find(name);
        if (anim == animations.end()) {
            throw std::invalid_argument("Animation not found (in play): " + name);
        }
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found: " + name);
        }
        meta->current_animation = anim->second;
        if (!meta->pause) {
            const auto current_time = std::chrono::steady_clock::now();
            if (meta->last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
                meta->last_time = current_time;
            }
            const auto delta_time = current_time - meta->last_time;
            meta->animation_duration += delta_time * this->acceleration;
            meta->last_time = current_time;

            auto raw_time = meta->animation_duration.count() * anim->second->tps;
            if (raw_time >= anim->second->duration) {
                if (repeat) {
                    raw_time = 0.0f;
                    meta->animation_duration = std::chrono::seconds(0);
                } else {
                    completed = true;
                    meta->pause = true;
                    raw_time = anim->second->duration;
                    if (completedCallback) {
                        completedCallback(this);
                    }
                }
            }

            const auto animation_time = raw_time;

            if (!meta->bone_transform.empty()) { // mam kosti
                this->updateBonesAnimation(anim->second, meta, animation_time);
            } else {
                const glm::vec3 scale = anim->second->nodes.begin()->get()->scalingLerp(animation_time);
                const glm::vec3 position = anim->second->nodes.begin()->get()->positionLerp(animation_time);
                const auto rotation = anim->second->nodes.begin()->get()->rotationLerp(animation_time);
                const auto alpha = anim->second->nodes.begin()->get()->alphaLerp(animation_time);

                meta->alpha = alpha;

                const auto translate = glm::translate(glm::mat4(1.f), position);
                const auto rotate = glm::mat4_cast(rotation);
                const auto scale_mat = glm::scale(glm::mat4(1.f), scale);

                meta->world_transform = translate * rotate * scale_mat;
            }
        }  else if (meta) {
            meta->last_time = std::chrono::steady_clock::now();
        }

        return meta;
    }

    vector<shared_ptr<Mesh>> AnimationPlayer::getNoBonesMeshes() const {
        return noBonesMeshes;
    }

    vector<shared_ptr<Mesh>> AnimationPlayer::getMeshes() const {
        return meshes;
    }

    bool AnimationPlayer::isCompleted() const {
        return completed;
    }

    void AnimationPlayer::setRepeat(const bool repeat) {
        this->repeat = repeat;
    }

    void AnimationPlayer::setCompletedCallback(const std::function<void(AnimationPlayer*)> &callback) {
        completedCallback = callback;
    }

    shared_ptr<AnimationMeta> AnimationPlayer::getMetadata(const string &name) const {
        const auto anim = animations.find(name);
        if (anim == animations.end()) {
            throw std::invalid_argument("Animation not found (in getMetadata): " + name);
        }
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found:" + name);
        }
        meta->current_animation = anim->second;

        return meta;
    }

    shared_ptr<AnimationPlayer> AnimationPlayer::clone() const {
        auto copy = make_shared<AnimationPlayer>();
        copy->animations = animations;
        copy->meshes = meshes;
        copy->bones = bones;
        copy->noBonesMeshes = noBonesMeshes;
        copy->bones_map = bones_map;
        copy->skeleton = skeleton;
        copy->global_inverse = global_inverse;
        copy->acceleration = acceleration;
        copy->repeat = repeat;
        copy->completed = false;

        for (const auto &[name, meta] : metadata) {
            auto newMeta = make_shared<AnimationMeta>();
            newMeta->name = name;
            newMeta->animation_duration = std::chrono::seconds(0);
            newMeta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
            newMeta->bone_transform.resize(meta->bone_transform.size(), glm::mat4(1.0f));
            newMeta->pause = true;
            newMeta->alpha = 1.0f;
            newMeta->world_transform = glm::mat4(1.0f);
            copy->metadata.emplace(name, newMeta);
        }

        return copy;
    }

    void AnimationPlayer::reset(const string &name) {
        completed = false;
        const auto meta = metadata.at(name);
        if (!meta) {
            throw std::invalid_argument("Animation metadata not found: " + name);
        }
        meta->animation_duration = std::chrono::seconds(0);
        meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
        meta->pause = false;
        meta->alpha = 1.0f;
        meta->world_transform = glm::mat4(1.0f);
    }

    void AnimationPlayer::updateBonesAnimation(
        const shared_ptr<Animation> &anim, const shared_ptr<AnimationMeta> &meta, const double animation_time) const {
        function<void(const Tree<uint32_t> &, const glm::mat4 &)> node_traversal;
        node_traversal = [&](const Tree<uint32_t> &node, const glm::mat4 &parent_mat) {
            const auto anim_node = findAnimationNode(anim, bones[*node]);
            auto local_transform = !bones[*node]->isFake()
                                       ? bones[*node]->node_transform
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
                meta->bone_transform[*node] = parent_mat * local_transform * (bones[*node])->offset_matrix;
            } else {
                meta->bone_transform[*node] = local_transform;
            }

            for (const auto &n: node) {
                node_traversal(n, transform);
            }
        };

        try {
            node_traversal(*skeleton, glm::mat4(1.f));
        } catch (const std::exception &e) {
            throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
        }
    }

    shared_ptr<AnimationNode> AnimationPlayer::findAnimationNode(
        const shared_ptr<Animation> &animation, const shared_ptr<Bone> &bone) {
        for (const auto &node: animation->nodes) {
            if (node->bone == bone) {
                return node;
            }
        }

        return nullptr;
    }
} // Animation