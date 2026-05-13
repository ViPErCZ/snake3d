#ifndef SNAKE3_JUMPTRAJECTORY_H
#define SNAKE3_JUMPTRAJECTORY_H

#include <glm/glm.hpp>

namespace Physic::Jump {

    class JumpTrajectory {
    public:
        // duration: total flight time (seconds); must be > 0.
        // peakHeight: the highest point of the arc, measured along `upAxis` above the
        //             start-end line; gravity is derived to satisfy h = g*T^2 / 8.
        // upAxis: world up direction (defaults to +Z, which is "up" in this game).
        JumpTrajectory(const glm::vec3 &start,
                       const glm::vec3 &end,
                       float duration,
                       float peakHeight,
                       const glm::vec3 &upAxis = glm::vec3(0.0f, 0.0f, 1.0f));

        [[nodiscard]] glm::vec3 positionAt(float t) const;

        [[nodiscard]] float getDuration() const { return duration; }
        [[nodiscard]] float getPeakHeight() const { return peakHeight; }
        [[nodiscard]] float getGravity() const { return gravity; }
        [[nodiscard]] bool isFinished(float t) const { return t >= duration; }

    private:
        glm::vec3 start;
        glm::vec3 horizontalVelocity{};
        glm::vec3 upAxis;
        float upDelta;
        float duration;
        float peakHeight;
        float gravity;
        float initialUpVelocity;
    };

} // namespace Physic::Jump

#endif //SNAKE3_JUMPTRAJECTORY_H
