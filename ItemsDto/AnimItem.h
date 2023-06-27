#ifndef SNAKE3_ANIMITEM_H
#define SNAKE3_ANIMITEM_H

#include "BaseItem.h"
#include <memory>
#include <vector>
#include <glm/gtx/quaternion.hpp>

using namespace std;

namespace ItemsDto {
    template <typename T>
    struct KeyFrame {
        T data;
        double time;

        KeyFrame(T data, double time) noexcept
                : data{std::move(data)}
                , time(time) {}
    };

    struct Bone {
        std::string name;
        glm::mat4 node_transform {1.0f};
        glm::mat4 offset_matrix {1.0f};

        Bone(string name, const glm::mat4& _offset_matrix) noexcept
                : name {std::move(name)}
                , offset_matrix {_offset_matrix} {
        }

        [[nodiscard]] auto isFake() const noexcept { return name.at(0) == '<'; }
    };

    struct VertexBoneWeight {
        static constexpr uint32_t BONE_COUNT = 4;

        std::array<uint32_t, BONE_COUNT> bone_index;
        std::array<float, BONE_COUNT> weight;

        void addBoneWeight(uint32_t id, float w) noexcept {
            uint32_t i;
            for (i = 0; i < BONE_COUNT && weight[i] != 0.0f; ++i);

            if (i >= BONE_COUNT) {/* no more weight slots */ return; }

            bone_index[i] = id;
            weight[i] = w;
        }
    };

    struct AnimationNode {
        vector<KeyFrame<glm::fquat>> rotations;
        vector<KeyFrame<glm::vec3>> positions;
        vector<KeyFrame<glm::vec3>> scales;
        Bone &bone;

        AnimationNode(decltype(positions) positions, decltype(rotations) rotations, decltype(scales) scales, Bone &_bone) noexcept;

        [[nodiscard]] size_t findPositionKeyframe(double anim_time) const;
        [[nodiscard]] size_t findRotationKeyframe(double anim_time) const;
        [[nodiscard]] size_t findScalingKeyframe(double anim_time) const;
        [[nodiscard]] glm::vec3 positionLerp(double anim_time) const;
        [[nodiscard]] glm::fquat rotationLerp(double anim_time) const;
        [[nodiscard]] glm::vec3 scalingLerp(double anim_time) const;
    };

    struct Animation {
        std::vector<AnimationNode> nodes;
        std::string name;
        double duration;
        double tps;

        Animation(std::string name, double duration, double tps, decltype(nodes) nodes) noexcept
                : nodes(std::move(nodes))
                , name(std::move(name))
                , duration(duration)
                , tps(tps) {
        }
    };
}


#endif //SNAKE3_ANIMITEM_H
