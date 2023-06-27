#include "AnimItem.h"

ItemsDto::AnimationNode::AnimationNode(decltype(positions) _positions, decltype(rotations) _rotations, decltype(scales) _scales, Bone& _bone) noexcept
        : rotations(std::move(_rotations))
        , positions(std::move(_positions))
        , scales(std::move(_scales))
        , bone(_bone) {
}

glm::vec3 ItemsDto::AnimationNode::positionLerp(double anim_time) const {
    if (positions.empty()) {
        return glm::vec3{0.0f};
    }

    if (positions.size() == 1) {
        return positions[0].data;
    }

    auto index = findPositionKeyframe(anim_time);
    auto& a = positions[index];
    auto& b = positions[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

//    return glm::mix(a.data, b.data, norm);
    return a.data * (float)(1.0 - norm) + b.data * (float)norm;
}

glm::fquat ItemsDto::AnimationNode::rotationLerp(double anim_time) const {
    if (rotations.empty()) {
        return glm::fquat{1.f, 0.f, 0.f, 0.f};
    }
    if (rotations.size() == 1) {
        return rotations[0].data;
    }

    auto index = findRotationKeyframe(anim_time);
    auto& a = rotations[index];
    auto& b = rotations[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

    return glm::normalize(glm::slerp(a.data, b.data, static_cast<float>(norm)));
}

glm::vec3 ItemsDto::AnimationNode::scalingLerp(double anim_time) const {
    if (scales.empty()) {
        return glm::vec3{1.f};
    }

    if (scales.size() == 1) {
        return scales[0].data;
    }

    auto index = findScalingKeyframe(anim_time);
    auto a = scales[index];
    auto b = scales[index + 1];

    double dt = b.time - a.time;
    double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

    return glm::mix(a.data, b.data, norm);
}

size_t ItemsDto::AnimationNode::findPositionKeyframe(double anim_time) const {
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        if (anim_time <= positions[i + 1].time) {
            return i;
        }
    }
    return 0;
    throw std::out_of_range("no position keyframe for time " + std::to_string(anim_time));
}

size_t ItemsDto::AnimationNode::findRotationKeyframe(double anim_time) const {
    for (size_t i = 0; i < rotations.size() - 1; ++i) {
        if (anim_time <= rotations[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no rotation keyframe for time " + std::to_string(anim_time));
}

size_t ItemsDto::AnimationNode::findScalingKeyframe(double anim_time) const {
    for (size_t i = 0; i < scales.size() - 1; ++i) {
        if (anim_time <= scales[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no scaling keyframe for time " + std::to_string(anim_time));
}
