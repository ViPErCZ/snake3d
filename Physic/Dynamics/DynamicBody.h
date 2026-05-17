#ifndef SNAKE3_DYNAMICBODY_H
#define SNAKE3_DYNAMICBODY_H

#include <glm/glm.hpp>

namespace Physic::Dynamics {

    // A minimal kinematic body for gravity-driven motion.
    //
    // Holds its own position and linear velocity; the owning system calls
    // `integrate(dt, gravity)` each tick to advance state. No rotation, no
    // collision response - those are layered on top by the physics world.
    //
    // - `enabled` is the master switch (disabled body is fully frozen).
    // - `useGravity` lets a single body opt out of gravity (e.g. levitating
    //   objects) while still being integrated for any directly-set velocity.
    // - `gravityScale` lets a body fall faster or slower than the world default.
    class DynamicBody {
    public:
        DynamicBody() = default;
        explicit DynamicBody(const glm::vec3 &initialPosition);

        void setPosition(const glm::vec3 &p) { position = p; }
        [[nodiscard]] const glm::vec3 &getPosition() const { return position; }

        void setVelocity(const glm::vec3 &v) { velocity = v; }
        [[nodiscard]] const glm::vec3 &getVelocity() const { return velocity; }
        void addImpulse(const glm::vec3 &impulse) { velocity += impulse; }

        void setGravityScale(const float scale) { gravityScale = scale; }
        [[nodiscard]] float getGravityScale() const { return gravityScale; }

        void setUseGravity(const bool use) { useGravity = use; }
        [[nodiscard]] bool isUsingGravity() const { return useGravity; }

        void setEnabled(const bool e) { enabled = e; }
        [[nodiscard]] bool isEnabled() const { return enabled; }

        // Advances velocity (gravity acceleration if enabled) and integrates
        // position by `velocity * dt`. No-op when the body is disabled.
        void integrate(float dt, const glm::vec3 &gravity);

    private:
        glm::vec3 position{0.0f};
        glm::vec3 velocity{0.0f};
        float gravityScale = 1.0f;
        bool useGravity = true;
        bool enabled = true;
    };

} // namespace Physic::Dynamics

#endif //SNAKE3_DYNAMICBODY_H
