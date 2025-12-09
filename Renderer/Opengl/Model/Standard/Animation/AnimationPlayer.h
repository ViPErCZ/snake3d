#ifndef SNAKE3_ANIMATIONPLAYER_H
#define SNAKE3_ANIMATIONPLAYER_H

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "../../AnimationModel.h"
#include "../../../../../ItemsDto/AnimItem.h"
#include "../../../../../Manager/ShaderManager.h"

using namespace std;
using namespace ItemsDto;
using namespace Model;
using namespace Manager;

namespace Animations {
    class AnimationPlayer {
    public:
        AnimationPlayer() = default;
        explicit AnimationPlayer(const string &name);
        AnimationPlayer(const vector<shared_ptr<Mesh> > &meshes,
                        const map<string, shared_ptr<Animation>> &animations,
                        const vector<Bone> &bones, const Tree<uint32_t> &skeleton,
                        const unordered_map<std::string, uint32_t> &bones_map,
                        const glm::mat4 &global_matrix
        );

        void createAnimation(const string &name);

        void addAnimationNode(const string &name, const shared_ptr<AnimationNode> &animationNode);

        glm::mat4 play(const string &name, const shared_ptr<ShaderManager> &shader);

    private:
        map<string, shared_ptr<Animation> > animations;
        unordered_map<string, shared_ptr<AnimationMeta> > metadata;
        float acceleration = 1.0f;

        vector<shared_ptr<Mesh> > meshes;
        vector<Bone> bones;
        unordered_map<std::string, uint32_t> bones_map;
        optional<Tree<uint32_t> > skeleton;
        glm::mat4 global_inverse{};
    };
} // Animation

#endif //SNAKE3_ANIMATIONPLAYER_H
