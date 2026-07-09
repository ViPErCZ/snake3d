#ifndef SNAKE3_FRUSTUM_H
#define SNAKE3_FRUSTUM_H

#include <glm/glm.hpp>

namespace Tools {
    // A 6-plane view frustum extracted from a view-projection matrix (Gribb-Hartmann).
    // Each plane is stored as (nx, ny, nz, d) with the convention: a point p is INSIDE
    // the half-space when dot(n, p) + d >= 0. Used for conservative AABB culling in the
    // render traversal (skip a node's draw when its world AABB is fully outside).
    //
    // The same class serves every pass - the caller builds it from whatever
    // view-projection that pass renders with (camera for the main/refraction pass, the
    // mirrored camera for the reflection pass, the light-space matrix per shadow cascade).
    class Frustum {
    public:
        Frustum() = default;
        explicit Frustum(const glm::mat4 &viewProj);

        // Conservative test (may keep an off-screen box near a corner, never culls a
        // visible one): true if the AABB is at least partially inside all 6 planes.
        [[nodiscard]] bool intersectsAABB(const glm::vec3 &min, const glm::vec3 &max) const;

    private:
        glm::vec4 planes[6]{};
    };
} // Tools

#endif // SNAKE3_FRUSTUM_H
