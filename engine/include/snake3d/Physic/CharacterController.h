#ifndef SNAKE3_CHARACTERCONTROLLER_H
#define SNAKE3_CHARACTERCONTROLLER_H

#include <functional>
#include <memory>

#include <glm/glm.hpp>

namespace Model { class MeshNode3D; }

namespace Physic {
    class CollisionSystem3D;

    // Tunables for a kinematic walking character. Defaults match a ~1.8 m tall
    // first-person actor. Masks default to WORLD (bit 0 of Tools/Layers.h); a game
    // that tags wall SIDES with an extra "blocker" bit can split the two.
    struct CharacterControllerParams {
        // The resting CENTER sits `radius` above the floor (a sphere-like foot), so
        // position() + eyeOffset reads as a standing view; `height` is the full
        // standing height used only for head/ceiling clearance (feet = center-radius,
        // head = center + (height-radius)).
        float radius         = 0.4f;   // capsule radius (horizontal + lower half-extent)
        float height         = 1.8f;   // capsule total standing height (feet..head)
        float stepHeight     = 0.5f;   // max ledge auto-climbed / snapped-down (stairs, curbs, riverbank)
        float gravity        = 22.0f;  // |g| along -Z (world units / s^2)
        float groundedVelEps = 0.25f;  // |vel.z| under this while touching => grounded
        std::uint32_t blockerMask = 1u; // colliders that block horizontal motion (WORLD)
        std::uint32_t groundMask  = 1u; // colliders that count as floor/ceiling (WORLD)
        // Optional smooth ground surface (e.g. a heightfield) sampled under the feet:
        // returns the world Z of the ground at (x,y). The effective floor each frame is
        // max(box tops, groundHeightFn) so the character walks uphill/downhill smoothly
        // while box colliders (buildings, stairs) still work. Empty => box colliders only.
        std::function<float(float x, float y)> groundHeightFn{};
    };

    // Kinematic capsule character: collide-and-slide against the world via the
    // CollisionSystem3D read-only scene queries (overlapSphere / raycast). It does
    // NOT use DynamicBody integration or CollisionSystem3D::step() / resolveTopContact
    // — so it can do what the engine solver deliberately won't: STEP UP onto a ledge
    // entered from below (the solver only lands a body onto surfaces it fell onto).
    // The game owns input mapping, look/yaw, sprint, jump impulse, eye offset and
    // death/respawn; the controller owns gravity, wall-slide, step-up and ground snap.
    //
    // Per frame the game computes a horizontal "wish" velocity (units/s) from its
    // input and calls move(dt, wish); jump(impulse) requests an upward kick when
    // grounded. Assumes the node is at the scene root (no parent transform); node
    // scale is honoured when writing the resolved world position back.
    class CharacterController {
    public:
        CharacterController(std::shared_ptr<CollisionSystem3D> world,
                            std::shared_ptr<Model::MeshNode3D> node,
                            CharacterControllerParams params);

        // Advance one frame: apply gravity, slide/step horizontally by wishVelocity*dt,
        // then resolve floor/ceiling and snap to ground. wishVelocity is horizontal
        // (z ignored). Writes the resolved world position back onto the node.
        void move(float dt, const glm::vec3 &wishVelocity);

        // Request an upward velocity this frame. Only takes effect while grounded.
        void jump(float impulse);

        // Hard-place the capsule CENTER at a world position (e.g. spawn/respawn).
        void teleport(const glm::vec3 &centerPos);

        void setEnabled(const bool e) { enabled = e; }
        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] bool isGrounded() const { return grounded; }

        [[nodiscard]] glm::vec3 position() const { return center; }       // capsule center, world
        [[nodiscard]] glm::vec3 footPosition() const;                      // capsule bottom, world
        [[nodiscard]] glm::vec3 velocity() const { return velocity_; }
        void setVelocity(const glm::vec3 &v) { velocity_ = v; }

        [[nodiscard]] CharacterControllerParams &params() { return p; }
        [[nodiscard]] const CharacterControllerParams &params() const { return p; }

    private:
        void resolveHorizontal();           // wall push-out (MTV slide) + step-up
        void resolveVertical(bool wasGrounded); // floor snap / ceiling stop / step-down
        void writeBack() const;             // push resolved center onto the node

        std::shared_ptr<CollisionSystem3D> world;
        std::shared_ptr<Model::MeshNode3D> node;
        CharacterControllerParams p;

        glm::vec3 center{0.0f};    // capsule center in WORLD space (source of truth)
        glm::vec3 velocity_{0.0f}; // xy from last wish, z integrated by gravity/jump
        bool grounded = false;
        bool enabled = true;
    };
} // namespace Physic

#endif // SNAKE3_CHARACTERCONTROLLER_H
