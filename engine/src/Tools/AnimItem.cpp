#include <snake3d/Tools/AnimItem.h>

using namespace std;

Animation::AnimationNode::AnimationNode(decltype(positions) positions, decltype(rotations) rotations, decltype(scales) scales, const shared_ptr<Bone> &_bone) noexcept
        : rotations(std::move(rotations))
        , positions(std::move(positions))
        , scales(std::move(scales))
        , bone(_bone) {
}

Animation::AnimationNode::AnimationNode(decltype(positions) positions, decltype(rotations) rotations, decltype(scales) scales) noexcept
    : rotations(std::move(rotations))
    , positions(std::move(positions))
    , scales(std::move(scales)) {
}

void Animation::AnimationNode::setAlphaFrames(decltype(alphas) alphas) noexcept {
    this->alphas = std::move(alphas);
}

glm::vec3 Animation::AnimationNode::positionLerp(const double anim_time) const {
    if (positions.empty()) {
        return glm::vec3{0.0f};
    }

    if (positions.size() == 1) {
        return positions[0].data;
    }

    const auto index = findPositionKeyframe(anim_time);
    auto& a = positions[index];
    auto& b = positions[index + 1];

    const double dt = b.time - a.time;
    const double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }
    const double eased = ease(norm, easing_value);

    return a.data * static_cast<float>(1.0 - eased) + b.data * static_cast<float>(eased);

//    return glm::mix(a.data, b.data, norm);
    //return a.data * static_cast<float>(1.0 - norm) + b.data * static_cast<float>(norm);
}

glm::fquat Animation::AnimationNode::rotationLerp(const double anim_time) const {
    if (rotations.empty()) {
        return glm::fquat{1.f, 0.f, 0.f, 0.f};
    }
    if (rotations.size() == 1) {
        return rotations[0].data;
    }

    const auto index = findRotationKeyframe(anim_time);
    auto& a = rotations[index];
    auto& b = rotations[index + 1];

    const double dt = b.time - a.time;
    const double norm = (anim_time - a.time) / dt;

//    if (!(norm >= 0.f && norm <= 1.f)) {
//        throw std::runtime_error("norm >= 0.f && norm <= 1.f");
//    }

    return glm::normalize(glm::slerp(a.data, b.data, static_cast<float>(norm)));
}

float Animation::AnimationNode::alphaLerp(const double anim_time) const {
    if (alphas.empty()) {
        return 1.0f;
    }

    if (alphas.size() == 1) {
        return alphas[0].data;
    }

    const size_t index = findAlphaKeyframe(anim_time);
    const auto& a = alphas[index];
    const auto& b = alphas[index + 1];

    const double dt = b.time - a.time;
    if (dt <= 0.0) {
        return b.data;
    }

    double t = (anim_time - a.time) / dt;
    t = glm::clamp(t, 0.0, 1.0);
    t = ease(t, easing_value);

    return static_cast<float>(a.data + (b.data - a.data) * t);
}

glm::vec3 Animation::AnimationNode::scalingLerp(const double anim_time) const {
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

double Animation::AnimationNode::ease(double t, const double curve) {
    if (t < 0.0) t = 0.0;
    else if (t > 1.0) t = 1.0;

    if (curve > 0.0) {
        return 1.0 - std::pow(1.0 - t, 1.0 + curve * 3.0);
    }
    if (curve < 0.0) {
        return std::pow(t, 1.0 - curve * 3.0);
    }
    return t;
}

size_t Animation::AnimationNode::findPositionKeyframe(const double anim_time) const {
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        if (anim_time <= positions[i + 1].time) {
            return i;
        }
    }
    return 0;
    throw std::out_of_range("no position keyframe for time " + std::to_string(anim_time));
}

size_t Animation::AnimationNode::findRotationKeyframe(const double anim_time) const {
    for (size_t i = 0; i < rotations.size() - 1; ++i) {
        if (anim_time <= rotations[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no rotation keyframe for time " + std::to_string(anim_time));
}

size_t Animation::AnimationNode::findScalingKeyframe(const double anim_time) const {
    for (size_t i = 0; i < scales.size() - 1; ++i) {
        if (anim_time <= scales[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no scaling keyframe for time " + std::to_string(anim_time));
}

size_t Animation::AnimationNode::findAlphaKeyframe(const double anim_time) const {
    for (size_t i = 0; i < alphas.size() - 1; ++i) {
        if (anim_time <= alphas[i + 1].time)
            return i;
    }
    return 0;
    throw std::out_of_range("no scaling keyframe for time " + std::to_string(anim_time));
}