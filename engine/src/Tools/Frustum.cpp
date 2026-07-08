#include <snake3d/Tools/Frustum.h>

namespace Tools {
    Frustum::Frustum(const glm::mat4 &m) {
        // glm is column-major: m[col][row]. Row i of the matrix = (m[0][i], m[1][i],
        // m[2][i], m[3][i]). Gribb-Hartmann: each clip plane is a sum/difference of
        // the w-row and one of the x/y/z rows.
        auto row = [&](const int i) { return glm::vec4(m[0][i], m[1][i], m[2][i], m[3][i]); };
        const glm::vec4 rx = row(0), ry = row(1), rz = row(2), rw = row(3);
        planes[0] = rw + rx; // left
        planes[1] = rw - rx; // right
        planes[2] = rw + ry; // bottom
        planes[3] = rw - ry; // top
        planes[4] = rw + rz; // near
        planes[5] = rw - rz; // far
        for (auto &p : planes) {
            if (const float len = glm::length(glm::vec3(p)); len > 1e-8f) p /= len;
        }
    }

    bool Frustum::intersectsAABB(const glm::vec3 &min, const glm::vec3 &max) const {
        for (const auto &p : planes) {
            const glm::vec3 n(p);
            // "positive vertex": the AABB corner farthest along the plane normal. If even
            // that corner is behind the plane, the whole box is outside -> cull.
            const glm::vec3 pv(n.x >= 0.0f ? max.x : min.x,
                               n.y >= 0.0f ? max.y : min.y,
                               n.z >= 0.0f ? max.z : min.z);
            if (glm::dot(n, pv) + p.w < 0.0f) return false;
        }
        return true;
    }
} // Tools
