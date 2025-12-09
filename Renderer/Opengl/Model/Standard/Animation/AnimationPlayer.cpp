#include "AnimationPlayer.h"

namespace Animations {
    AnimationPlayer::AnimationPlayer(const string &name) {
        createAnimation(name);
    }

    AnimationPlayer::AnimationPlayer(const vector<shared_ptr<Mesh>> &meshes,
                                     const map<string, shared_ptr<Animation>> &animations, const vector<Bone> &bones,
                                     const Tree<uint32_t> &skeleton, const unordered_map<std::string, uint32_t> &bones_map,
                                     const glm::mat4 &global_matrix) :
        animations(animations), meshes(meshes), bones(bones), bones_map(bones_map), skeleton(skeleton),
        global_inverse(global_matrix) {
    }

    void AnimationPlayer::createAnimation(const string &name) {
        animations.emplace(name, make_shared<Animation>(name, 0, 25));
        const auto meta = make_shared<AnimationMeta>();
        meta->name = name;
        meta->animation_duration = std::chrono::seconds(0);
        meta->last_time = std::chrono::time_point<std::chrono::steady_clock>();
        meta->pause = false;
        metadata.emplace(name, meta);
    }

    // Node by melo byt svazano s objektem (animation objekt v obj muze mit vice node)
    // AnimationNode ma pak svoje key frames, kde se resi transformace (pripadne dodelame alpha blending)
    //
    void AnimationPlayer::addAnimationNode(const string &name, const shared_ptr<AnimationNode> &animationNode) {
        const auto anim = animations.find(name);
        if (anim == animations.end()) {
            throw std::invalid_argument("Animation not found");
        }

        const auto meta = metadata.at(name);
        if (nullptr == meta) {
            throw std::invalid_argument("Animation metadata not found");
        }

        anim->second->duration = 32; //(animationNode->positions.end()-1)->time;
        //meta->animation_duration = std::chrono::seconds(0);
        anim->second->nodes.emplace_back(animationNode);
    }

    glm::mat4 AnimationPlayer::play(const string &name, const shared_ptr<ShaderManager> &shader) {
        const auto anim = animations.find(name);
        if (anim == animations.end()) {
            throw std::invalid_argument("Animation not found");
        }
        const auto meta = metadata.at(name);
        if (!meta->pause) {
            const auto current_time = std::chrono::steady_clock::now();
            if (meta->last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
                meta->last_time = current_time;
            }
            const auto delta_time = current_time - meta->last_time;
            meta->animation_duration += delta_time * this->acceleration;
            meta->last_time = current_time;

            // TOTO ZAJISTUJE, ze se animace prehrava cyklicky
            // pokud by animace mela byt jen jednou, tak se to muze odstranit
            auto animation_time = glm::mod(meta->animation_duration.count() * anim->second->tps, anim->second->duration);
            // const auto animation_time = meta->animation_duration.count() * anim.tps;
            if (animation_time >= anim->second->duration) {
                meta->animation_duration = std::chrono::seconds(0);;
                animation_time = 0;
            }

            // TODO: AnimationModel nejak integrovat do animation playeru
            // bud mam kosti nebo mam to mam jen positional animaci vcetne timer uniformu aktualniho shaderu
            if (!meta->bone_transform.empty()) { // mam kosti
                cout << "bones animation model" << endl;
            } else {
                const glm::vec3 scale = anim->second->nodes.begin()->get()->scalingLerp(animation_time);
                const glm::vec3 position = anim->second->nodes.begin()->get()->positionLerp(animation_time);
                const auto rotation = anim->second->nodes.begin()->get()->rotationLerp(animation_time);
                const auto alpha = anim->second->nodes.begin()->get()->alphaLerp(animation_time);

                shader->setUniform("alpha", alpha);

                const auto translate = glm::translate(glm::mat4(1.f), position);
                const auto rotate = glm::mat4_cast(rotation);
                const auto scale_mat = glm::scale(glm::mat4(1.f), scale);

                return translate * rotate * scale_mat;
            }
        }  else if (meta) {
            meta->last_time = std::chrono::steady_clock::now();
        }

        return glm::mat4(1.f);
    }
} // Animation