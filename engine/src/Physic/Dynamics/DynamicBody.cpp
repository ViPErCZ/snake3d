#include <snake3d/Physic/Dynamics/DynamicBody.h>

namespace Physic::Dynamics {

    DynamicBody::DynamicBody(const glm::vec3 &initialPosition)
        : position(initialPosition) {}

    void DynamicBody::integrate(const float dt, const glm::vec3 &gravity) {
        if (!enabled || debugFrozen || dt <= 0.0f) {
            return;
        }
        if (useGravity) {
            // Symplectic Euler: gravity bumps velocity first, then position
            // integrates with the updated velocity. Stable for constant
            // acceleration and matches the discrete behavior people expect.
            velocity += gravity * gravityScale * dt;
        }
        position += velocity * dt;
    }

} // namespace Physic::Dynamics
