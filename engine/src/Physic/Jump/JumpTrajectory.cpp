#include <snake3d/Physic/Jump/JumpTrajectory.h>

#include <algorithm>

namespace Physic::Jump {

    JumpTrajectory::JumpTrajectory(const glm::vec3 &start,
                                   const glm::vec3 &end,
                                   const float duration,
                                   const float peakHeight,
                                   const glm::vec3 &upAxis)
        : start(start),
          upAxis(glm::normalize(upAxis)),
          upDelta(0.0f),
          duration(duration),
          peakHeight(peakHeight),
          gravity(0.0f),
          initialUpVelocity(0.0f) {
        const glm::vec3 delta = end - start;
        upDelta = glm::dot(delta, this->upAxis);
        const glm::vec3 horizontalDelta = delta - this->upAxis * upDelta;
        horizontalVelocity = duration > 0.0f ? horizontalDelta / duration : glm::vec3(0.0f);

        // peakHeight = g * T^2 / 8  =>  g = 8 * h / T^2
        // initial vertical velocity v0 = g * T / 2 (parabola returns to baseline at T).
        gravity = duration > 0.0f ? (8.0f * peakHeight) / (duration * duration) : 0.0f;
        initialUpVelocity = 0.5f * gravity * duration;
    }

    glm::vec3 JumpTrajectory::positionAt(const float t) const {
        const float clamped = std::clamp(t, 0.0f, duration);
        const float linearUp = duration > 0.0f ? (clamped / duration) * upDelta : 0.0f;
        const float parabolicUp = initialUpVelocity * clamped - 0.5f * gravity * clamped * clamped;
        return start + horizontalVelocity * clamped + upAxis * (linearUp + parabolicUp);
    }

} // namespace Physic::Jump
