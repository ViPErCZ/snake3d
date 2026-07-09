#include <snake3d/Physic/CharacterController.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include <snake3d/Physic/CollisionSystem3D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Physic {

    CharacterController::CharacterController(std::shared_ptr<CollisionSystem3D> world,
                                             std::shared_ptr<Model::MeshNode3D> node,
                                             CharacterControllerParams params)
        : world(std::move(world)), node(std::move(node)), p(std::move(params)) {
        if (this->node) {
            const glm::vec3 s = this->node->getScale();
            center = this->node->getPosition() * s;
        }
    }

    glm::vec3 CharacterController::footPosition() const {
        return center - glm::vec3(0.0f, 0.0f, p.radius);
    }

    void CharacterController::jump(const float impulse) {
        if (!grounded) return;
        velocity_.z = impulse;
        grounded = false;
    }

    void CharacterController::teleport(const glm::vec3 &centerPos) {
        center = centerPos;
        velocity_ = glm::vec3(0.0f);
        grounded = false;
        writeBack();
    }

    void CharacterController::move(const float dt, const glm::vec3 &wishVelocity) {
        if (!enabled || dt <= 0.0f) return;

        // Anti-tunnel: split the frame so no single integration step displaces the
        // capsule by more than ~half its radius. A larger step could push the center
        // PAST a wall face in one go — and once the center is inside a box the overlap
        // normal degenerates to +Z (up), which the horizontal pass skips, so the body
        // would walk straight through. Substepping keeps every step shallow.
        const float maxDisp = p.radius * 0.5f;
        const float horizSpeed = std::sqrt(wishVelocity.x * wishVelocity.x +
                                           wishVelocity.y * wishVelocity.y);
        const float vertSpeed = std::fabs(velocity_.z) + p.gravity * dt;
        const float disp = std::max(horizSpeed, vertSpeed) * dt;
        const int steps = std::min(8, std::max(1, static_cast<int>(std::ceil(disp / maxDisp))));
        const float sdt = dt / static_cast<float>(steps);

        for (int i = 0; i < steps; ++i) {
            const bool wasGrounded = grounded;
            // Horizontal velocity is driven by the game each frame; vertical is ours.
            velocity_.x = wishVelocity.x;
            velocity_.y = wishVelocity.y;
            velocity_.z -= p.gravity * sdt;

            // 1) Horizontal: advance in the XY plane, then slide off walls / step up.
            center.x += velocity_.x * sdt;
            center.y += velocity_.y * sdt;
            resolveHorizontal();

            // 2) Vertical: integrate gravity/jump, then snap to floor or stop at ceiling.
            center.z += velocity_.z * sdt;
            resolveVertical(wasGrounded);
        }
        writeBack();
    }

    // Push the capsule out of wall SIDES it entered; for a ledge no taller than
    // stepHeight, climb onto it instead of being blocked (engine resolveTopContact
    // refuses to lift a body that entered a surface from below — this is that fix).
    void CharacterController::resolveHorizontal() {
        if (!world) return;
        const float feetZ = center.z - p.radius;   // center sits radius above the floor
        // Probe at foot level so both full-height walls AND low steps are seen
        // (a center-height sphere would float over a curb).
        const glm::vec3 probe(center.x, center.y, feetZ + p.radius);

        float bestStepTop = -std::numeric_limits<float>::infinity();
        for (const auto &hit : world->overlapSphere(probe, p.radius, p.blockerMask)) {
            if (std::fabs(hit.normal.z) > 0.5f) continue; // floor / ceiling, not a side
            const float top = hit.worldAABB.max.z;
            // A low ledge we may be able to step onto — defer (handled after the loop).
            if (top > feetZ + 1e-3f && top <= feetZ + p.stepHeight + 1e-3f) {
                bestStepTop = std::max(bestStepTop, top);
                continue;
            }
            // A real wall: slide out along the horizontal minimum-translation normal.
            glm::vec3 n(hit.normal.x, hit.normal.y, 0.0f);
            const float nlen = glm::length(n);
            if (nlen < 1e-4f) continue;
            n /= nlen;
            center += n * hit.depth;
            const float vn = velocity_.x * n.x + velocity_.y * n.y;
            if (vn < 0.0f) { velocity_.x -= n.x * vn; velocity_.y -= n.y * vn; }
        }

        // Step-up: lift onto the highest low ledge if there's headroom for the capsule.
        if (bestStepTop > -std::numeric_limits<float>::infinity()) {
            const glm::vec3 lifted(center.x, center.y, bestStepTop + p.radius + 0.05f);
            bool blocked = false;
            for (const auto &h : world->overlapSphere(lifted, p.radius * 0.9f, p.blockerMask)) {
                // Reject only a real OVERHANG: a blocker whose underside sits above the
                // step yet within head height. A box rising from at/below the step (a
                // taller stair tread, a wall flush with the ground) is climbable ground,
                // not a ceiling — otherwise the next step of a staircase would always
                // veto the step-up and the character would jam at the bottom.
                if (h.worldAABB.min.z > bestStepTop + 0.05f &&
                    h.worldAABB.min.z < bestStepTop + p.height) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                center.z = bestStepTop + p.radius;
                if (velocity_.z < 0.0f) velocity_.z = 0.0f;
                grounded = true;
            }
        }
    }

    void CharacterController::resolveVertical(const bool wasGrounded) {
        if (!world) return;
        const float feetZ = center.z - p.radius;            // center sits radius above the floor

        // --- Floor: a sphere at the feet catches a surface we rest on / penetrated.
        const glm::vec3 footProbe(center.x, center.y, feetZ + p.radius);
        constexpr float kNegInf = -std::numeric_limits<float>::infinity();
        float bestTop = kNegInf;
        for (const auto &hit : world->overlapSphere(footProbe, p.radius, p.groundMask)) {
            const float top = hit.worldAABB.max.z;
            // Only surfaces around/below the feet count as ground (not a wall beside us).
            if (top <= feetZ + p.radius + 1e-3f) bestTop = std::max(bestTop, top);
        }
        // Optional smooth ground (heightfield) directly under the feet — the effective
        // floor is the higher of the box tops and the terrain so the player walks up
        // hills/down slopes smoothly while still standing on boxes (buildings, stairs).
        const float terrainTop = p.groundHeightFn ? p.groundHeightFn(center.x, center.y) : kNegInf;
        const float groundTop = std::max(bestTop, terrainTop);

        grounded = false;
        if (groundTop > kNegInf && feetZ <= groundTop + 0.02f) {
            // Resting on / sunk into the floor: snap feet to its top.
            center.z = groundTop + p.radius;
            if (velocity_.z < 0.0f) velocity_.z = 0.0f;
            grounded = true;
        } else if (wasGrounded && velocity_.z <= 0.0f) {
            // Walked off a low ledge / down a slope: snap down to whichever ground is
            // highest within stepHeight below the feet, so we stick to stairs/descents
            // (and to the terrain when descending) instead of launching into a fall.
            float snapTo = kNegInf;
            if (terrainTop > kNegInf && feetZ - terrainTop <= p.stepHeight)
                snapTo = std::max(snapTo, terrainTop);
            if (const auto hit = world->raycast(footProbe, glm::vec3(0.0f, 0.0f, -1.0f),
                                                 p.stepHeight + p.radius, p.groundMask))
                snapTo = std::max(snapTo, hit->point.z);
            if (snapTo > kNegInf) {
                center.z = snapTo + p.radius;
                velocity_.z = 0.0f;
                grounded = true;
            }
        }

        // --- Ceiling: stop upward motion if the head hits something solid.
        if (velocity_.z > 0.0f) {
            const float headHalf = std::max(p.radius, p.height - p.radius); // center -> head top
            const float headZ = center.z + headHalf;
            const glm::vec3 headProbe(center.x, center.y, headZ - p.radius);
            for (const auto &hit : world->overlapSphere(headProbe, p.radius, p.groundMask)) {
                const float bot = hit.worldAABB.min.z;
                if (bot >= headZ - p.radius - 1e-3f) {
                    center.z = bot - headHalf;
                    velocity_.z = 0.0f;
                    break;
                }
            }
        }
    }

    void CharacterController::writeBack() const {
        if (!node) return;
        const glm::vec3 s = node->getScale();
        const glm::vec3 local(s.x != 0.0f ? center.x / s.x : center.x,
                              s.y != 0.0f ? center.y / s.y : center.y,
                              s.z != 0.0f ? center.z / s.z : center.z);
        node->setPosition(local);
        node->computeWorldMatrix(glm::mat4(1.0f));
    }
} // namespace Physic
