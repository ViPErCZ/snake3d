#include "AnimationModel.h"

namespace Model {
    AnimationModel::AnimationModel(const vector<Mesh *> &meshes,
                                   decltype(animations)&& _animations,
                                   decltype(bones)&& bones,
                                   decltype(skeleton)&& skeleton,
                                   decltype(bones_map)&& bones_map,
                                   const glm::mat4& _global_matrix) :
        meshes(meshes), animations(std::move(_animations)), bones(std::move(bones)), skeleton(skeleton), bones_map(bones_map), global_inverse {_global_matrix} {
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
} // Model