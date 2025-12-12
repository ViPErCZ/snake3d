#ifndef SNAKE3_ANIMATIONPLAYER_H
#define SNAKE3_ANIMATIONPLAYER_H

#include <chrono>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "../../../../../ItemsDto/AnimItem.h"
#include "../../Utils/Mesh.h"
#include "../../Utils/Tree.h"

using namespace std;
using namespace ItemsDto;
using namespace ModelUtils;

namespace Animations {
    struct AnimationMeta {
        std::string name;
        chrono::time_point<std::chrono::steady_clock> last_time;
        chrono::duration<double> animation_duration;
        vector<glm::mat4> bone_transform;
        bool pause;
        float alpha;
        glm::mat4 world_transform;
        shared_ptr<Animation> current_animation{};
    };
    class AnimationPlayer {
    public:
        AnimationPlayer() = default;
        explicit AnimationPlayer(const string &name);
        AnimationPlayer(const vector<shared_ptr<Mesh> > &meshes,
                        const map<string, shared_ptr<Animation>> &animations,
                        const vector<shared_ptr<Bone> > &bones, const Tree<uint32_t> &skeleton,
                        const unordered_map<std::string, uint32_t> &bones_map,
                        const glm::mat4 &global_matrix
        );

        void createAnimation(const string &name);

        void addAnimationNode(const string &name, const shared_ptr<AnimationNode> &animationNode, int duration);

        void setAcceleration(float acceleration);

        void start(const string &name) const;

        void stop(const string &name) const;
        // void pause(const string &name);
        // void resume(const string &name);

        shared_ptr<AnimationMeta> play(const string &name);

        vector<shared_ptr<Mesh> > getNoBonesMeshes() const;

        vector<shared_ptr<Mesh> > getMeshes() const;

    protected:
        void updateBonesAnimation(const shared_ptr<Animation> &anim, const shared_ptr<AnimationMeta> &meta) const;
        static shared_ptr<AnimationNode> findAnimationNode(const shared_ptr<Animation> &animation, const shared_ptr<Bone> &bone);

    private:
        map<string, shared_ptr<Animation> > animations;
        unordered_map<string, shared_ptr<AnimationMeta> > metadata;
        float acceleration = 1.0f;
        bool repeat = false;

        vector<shared_ptr<Mesh> > meshes;
        vector<shared_ptr<Bone> > bones;
        vector<shared_ptr<Mesh> > noBonesMeshes;
        unordered_map<std::string, uint32_t> bones_map;
        optional<Tree<uint32_t> > skeleton;
        glm::mat4 global_inverse{};
    };
} // Animation

#endif //SNAKE3_ANIMATIONPLAYER_H
