#ifndef SNAKE3_ANIMATIONMODEL_H
#define SNAKE3_ANIMATIONMODEL_H

#include <unordered_map>
#include "../../../ItemsDto/AnimItem.h"
#include "Utils/Mesh.h"
#include "Utils/Tree.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;

namespace Model {

    class AnimationModel {
    protected:
        vector<Mesh*> meshes;
        vector<Animation> animations;
        vector<Bone> bones;
        unordered_map<std::string, uint32_t> bones_map;
        Tree<uint32_t> skeleton;
        glm::mat4 global_inverse;
    public:
        AnimationModel(const vector<Mesh *> &meshes, decltype(animations)&& _animations, decltype(bones)&& bones, decltype(skeleton)&& skeleton, decltype(bones_map)&& bones_map,
                       const glm::mat4& _global_matrix);
        [[nodiscard]] const vector<Mesh *> &getMeshes() const;
        [[nodiscard]] const vector<Animation> &getAnimations() const;
        [[nodiscard]] const vector<Bone> &getBones() const;
        [[nodiscard]] const unordered_map<std::string, uint32_t> &getBonesMap() const;
        [[nodiscard]] const Tree<uint32_t> &getSkeleton() const;
        [[nodiscard]] const glm::mat4 &getGlobalInverse() const;
    };

} // Model

#endif //SNAKE3_ANIMATIONMODEL_H
