#ifndef SNAKE3_PICKING_H
#define SNAKE3_PICKING_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// Header-only mouse-picking helpers (pure glm, no GL or scene dependencies). A
// screen pixel is turned into a world ray via the inverse view-projection; from
// there ray/sphere (object pick), ray/ground-plane (click-to-place target) and the
// reverse world->screen projection (HUD anchoring, marquee selection) are provided.
// Reusable by any 3D game that needs clicking or to anchor 2D over 3D.
namespace Tools {

    struct Ray {
        glm::vec3 origin{0.0f};
        glm::vec3 dir{0.0f, 0.0f, -1.0f};
    };

    // Pixel (mx,my, top-left origin) -> world ray. view/proj are the current camera
    // matrices; w/h the framebuffer size.
    inline Ray screenRay(const double mx, const double my, const int w, const int h,
                         const glm::mat4 &view, const glm::mat4 &proj) {
        const float ndcX = 2.0f * static_cast<float>(mx) / static_cast<float>(w) - 1.0f;
        const float ndcY = 1.0f - 2.0f * static_cast<float>(my) / static_cast<float>(h); // flip Y
        const glm::mat4 inv = glm::inverse(proj * view);
        glm::vec4 pNear = inv * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 pFar  = inv * glm::vec4(ndcX, ndcY,  1.0f, 1.0f);
        pNear /= pNear.w;
        pFar  /= pFar.w;
        Ray r;
        r.origin = glm::vec3(pNear);
        r.dir = glm::normalize(glm::vec3(pFar - pNear));
        return r;
    }

    // Nearest ray/sphere hit in front of the origin. Returns t (distance) in `t`.
    inline bool raySphere(const Ray &r, const glm::vec3 &center, const float radius, float &t) {
        const glm::vec3 oc = r.origin - center;
        const float b = glm::dot(oc, r.dir);
        const float c = glm::dot(oc, oc) - radius * radius;
        const float disc = b * b - c;
        if (disc < 0.0f) return false;
        const float sq = std::sqrt(disc);
        const float t0 = -b - sq;
        t = (t0 >= 0.0f) ? t0 : (-b + sq);
        return t >= 0.0f;
    }

    // Ray vs the ground plane y = planeY. Returns the world hit point.
    inline bool rayGround(const Ray &r, const float planeY, glm::vec3 &hit) {
        if (std::fabs(r.dir.y) < 1e-6f) return false;
        const float t = (planeY - r.origin.y) / r.dir.y;
        if (t < 0.0f) return false;
        hit = r.origin + r.dir * t;
        return true;
    }

    // World point -> screen pixel (top-left origin). false if behind the camera.
    inline bool worldToScreen(const glm::vec3 &wp, const int w, const int h,
                              const glm::mat4 &view, const glm::mat4 &proj, glm::vec2 &out) {
        const glm::vec4 clip = proj * view * glm::vec4(wp, 1.0f);
        if (clip.w <= 0.0f) return false;
        const glm::vec3 ndc = glm::vec3(clip) / clip.w;
        out.x = (ndc.x * 0.5f + 0.5f) * static_cast<float>(w);
        out.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(h);
        return true;
    }

} // namespace Tools

#endif // SNAKE3_PICKING_H
