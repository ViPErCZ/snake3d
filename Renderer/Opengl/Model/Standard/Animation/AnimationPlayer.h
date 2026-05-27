#ifndef SNAKE3_ANIMATIONPLAYER_H
#define SNAKE3_ANIMATIONPLAYER_H

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "../../../../../Tools/AnimItem.h"
#include "../../Utils/Mesh.h"
#include "../../Utils/Tree.h"

using namespace std;
using namespace ModelUtils;

namespace Animation {
    struct AnimationMeta {
        std::string name;
        chrono::time_point<std::chrono::steady_clock> last_time;
        chrono::duration<double> animation_duration;
        vector<glm::mat4> bone_transform;
        bool pause;
        float alpha;
        glm::mat4 world_transform;
        shared_ptr<Animation::AnimationClip> current_animation{};
    };
    class AnimationPlayer {
    public:
        AnimationPlayer() = default;
        explicit AnimationPlayer(const string &name);
        AnimationPlayer(const vector<shared_ptr<Mesh> > &meshes,
                        const map<string, shared_ptr<Animation::AnimationClip>> &animations,
                        const vector<shared_ptr<Animation::Bone> > &bones, const Tree<uint32_t> &skeleton,
                        const unordered_map<std::string, uint32_t> &bones_map,
                        const glm::mat4 &global_matrix
        );

        void createAnimation(const string &name);

        void addAnimationNode(const string &name, const shared_ptr<Animation::AnimationNode> &animationNode, int duration);

        void setAcceleration(float acceleration);

        void start(const string &name, bool loop = true);

        void stop(const string &name) const;

        void pause(const string &name) const;

        void resume(const string &name) const;

        void reset(const string &name);

        shared_ptr<AnimationMeta> play(const string &name);

        vector<shared_ptr<Mesh> > getNoBonesMeshes() const;

        vector<shared_ptr<Mesh> > getMeshes() const;

        bool isCompleted() const;

        void setRepeat(bool repeat);

        void setCompletedCallback(const std::function<void(AnimationPlayer*)> &callback);

        shared_ptr<AnimationMeta> getMetadata(const string &name) const;

        shared_ptr<AnimationPlayer> clone() const;

    protected:
        void updateBonesAnimation(const shared_ptr<Animation::AnimationClip> &anim, const shared_ptr<AnimationMeta> &meta, double animation_time) const;
        static shared_ptr<Animation::AnimationNode> findAnimationNode(const shared_ptr<Animation::AnimationClip> &animation, const shared_ptr<Animation::Bone> &bone);

    private:
        map<string, shared_ptr<Animation::AnimationClip> > animations;
        unordered_map<string, shared_ptr<AnimationMeta> > metadata;
        float acceleration = 1.0f;
        bool repeat = false;
        bool completed = false;

        std::function<void(AnimationPlayer*)> completedCallback;
        vector<shared_ptr<Mesh> > meshes;
        vector<shared_ptr<Animation::Bone> > bones;
        vector<shared_ptr<Mesh> > noBonesMeshes;
        unordered_map<std::string, uint32_t> bones_map;
        optional<Tree<uint32_t> > skeleton;
        glm::mat4 global_inverse{};
    };
} // Animation

#endif //SNAKE3_ANIMATIONPLAYER_H
