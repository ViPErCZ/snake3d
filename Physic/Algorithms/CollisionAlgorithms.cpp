#include "CollisionAlgorithms.h"

#include "../BoxShape.h"
#include "../SphereShape.h"
#include "../CapsuleShape.h"
#include "../CylinderShape.h"

namespace {
    // Helper function to find closest point on segment p0-p1 to point q
    glm::vec3 ClosestPointOnSegment(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& q) {
        const glm::vec3 v = p1 - p0;
        const float t = glm::dot(q - p0, v) / glm::dot(v, v);

        return p0 + glm::clamp(t, 0.0f, 1.0f) * v;
    }

    // Helper to find closest points between two segments
    void ClosestPointsTwoSegments(const glm::vec3& p1, const glm::vec3& q1,
                                 const glm::vec3& p2, const glm::vec3& q2,
                                 glm::vec3& c1, glm::vec3& c2) {
        const glm::vec3 d1 = q1 - p1;
        const glm::vec3 d2 = q2 - p2;
        const glm::vec3 r = p1 - p2;
        const float a = glm::dot(d1, d1);
        const float e = glm::dot(d2, d2);
        const float f = glm::dot(d2, r);

        constexpr float EPSILON = 1e-6f;
        float s, t;

        if (a <= EPSILON && e <= EPSILON) {
            s = t = 0.0f;
            c1 = p1;
            c2 = p2;
            return;
        }
        if (a <= EPSILON) {
            s = 0.0f;
            t = glm::clamp(f / e, 0.0f, 1.0f);
        } else {
            const float c = glm::dot(d1, r);
            if (e <= EPSILON) {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            } else {
                const float b = glm::dot(d1, d2);
                const float denom = a * e - b * b;
                if (denom != 0.0f) {
                    s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                } else {
                    s = 0.0f;
                }
                t = (b * s + f) / e;
                if (t < 0.0f) {
                    t = 0.0f;
                    s = glm::clamp(-c / a, 0.0f, 1.0f);
                } else if (t > 1.0f) {
                    t = 1.0f;
                    s = glm::clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }
        c1 = p1 + d1 * s;
        c2 = p2 + d2 * t;
    }
}

bool Algorithms::BoxVsBox(const CollisionEntry &entA, const CollisionEntry &entB, const glm::mat4& worldA, const glm::mat4& worldB) {
    const auto obbA = std::static_pointer_cast<BoxShape>(
        entA.shapeNode->getShape())->BuildOBB(worldA);
    const auto obbB = std::static_pointer_cast<BoxShape>(
        entB.shapeNode->getShape())->BuildOBB(worldB);

    const glm::vec3 T = obbB.center - obbA.center;

    glm::mat3 R;
    glm::mat3 AbsR;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = glm::dot(obbA.axes[i], obbB.axes[j]);
            AbsR[i][j] = glm::abs(R[i][j]) + 1e-6f;
        }
    }

    float ra, rb;

    // Test axes L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++) {
        ra = obbA.halfExtents[i];
        rb = obbB.halfExtents[0] * AbsR[i][0] + obbB.halfExtents[1] * AbsR[i][1] + obbB.halfExtents[2] * AbsR[i][2];
        if (glm::abs(glm::dot(T, obbA.axes[i])) > ra + rb) return false;
    }

    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++) {
        ra = obbA.halfExtents[0] * AbsR[0][i] + obbA.halfExtents[1] * AbsR[1][i] + obbA.halfExtents[2] * AbsR[2][i];
        rb = obbB.halfExtents[i];
        if (glm::abs(glm::dot(T, obbB.axes[i])) > ra + rb) return false;
    }

    // Test axis L = A0 x B0
    ra = obbA.halfExtents[1] * AbsR[2][0] + obbA.halfExtents[2] * AbsR[1][0];
    rb = obbB.halfExtents[1] * AbsR[0][2] + obbB.halfExtents[2] * AbsR[0][1];
    if (glm::abs(glm::dot(T, obbA.axes[2]) * R[1][0] - glm::dot(T, obbA.axes[1]) * R[2][0]) > ra + rb) return false;

    // Test axis L = A0 x B1
    ra = obbA.halfExtents[1] * AbsR[2][1] + obbA.halfExtents[2] * AbsR[1][1];
    rb = obbB.halfExtents[0] * AbsR[0][2] + obbB.halfExtents[2] * AbsR[0][0];
    if (glm::abs(glm::dot(T, obbA.axes[2]) * R[1][1] - glm::dot(T, obbA.axes[1]) * R[2][1]) > ra + rb) return false;

    // Test axis L = A0 x B2
    ra = obbA.halfExtents[1] * AbsR[2][2] + obbA.halfExtents[2] * AbsR[1][2];
    rb = obbB.halfExtents[0] * AbsR[0][1] + obbB.halfExtents[1] * AbsR[0][0];
    if (glm::abs(glm::dot(T, obbA.axes[2]) * R[1][2] - glm::dot(T, obbA.axes[1]) * R[2][2]) > ra + rb) return false;

    // Test axis L = A1 x B0
    ra = obbA.halfExtents[0] * AbsR[2][0] + obbA.halfExtents[2] * AbsR[0][0];
    rb = obbB.halfExtents[1] * AbsR[1][2] + obbB.halfExtents[2] * AbsR[1][1];
    if (glm::abs(glm::dot(T, obbA.axes[0]) * R[2][0] - glm::dot(T, obbA.axes[2]) * R[0][0]) > ra + rb) return false;

    // Test axis L = A1 x B1
    ra = obbA.halfExtents[0] * AbsR[2][1] + obbA.halfExtents[2] * AbsR[0][1];
    rb = obbB.halfExtents[0] * AbsR[1][2] + obbB.halfExtents[2] * AbsR[1][0];
    if (glm::abs(glm::dot(T, obbA.axes[0]) * R[2][1] - glm::dot(T, obbA.axes[2]) * R[0][1]) > ra + rb) return false;

    // Test axis L = A1 x B2
    ra = obbA.halfExtents[0] * AbsR[2][2] + obbA.halfExtents[2] * AbsR[0][2];
    rb = obbB.halfExtents[0] * AbsR[1][1] + obbB.halfExtents[1] * AbsR[1][0];
    if (glm::abs(glm::dot(T, obbA.axes[0]) * R[2][2] - glm::dot(T, obbA.axes[2]) * R[0][2]) > ra + rb) return false;

    // Test axis L = A2 x B0
    ra = obbA.halfExtents[0] * AbsR[1][0] + obbA.halfExtents[1] * AbsR[0][0];
    rb = obbB.halfExtents[1] * AbsR[2][2] + obbB.halfExtents[2] * AbsR[2][1];
    if (glm::abs(glm::dot(T, obbA.axes[1]) * R[0][0] - glm::dot(T, obbA.axes[0]) * R[1][0]) > ra + rb) return false;

    // Test axis L = A2 x B1
    ra = obbA.halfExtents[0] * AbsR[1][1] + obbA.halfExtents[1] * AbsR[0][1];
    rb = obbB.halfExtents[0] * AbsR[2][2] + obbB.halfExtents[2] * AbsR[2][0];
    if (glm::abs(glm::dot(T, obbA.axes[1]) * R[0][1] - glm::dot(T, obbA.axes[0]) * R[1][1]) > ra + rb) return false;

    // Test axis L = A2 x B2
    ra = obbA.halfExtents[0] * AbsR[1][2] + obbA.halfExtents[1] * AbsR[0][2];
    rb = obbB.halfExtents[0] * AbsR[2][1] + obbB.halfExtents[1] * AbsR[2][0];
    if (glm::abs(glm::dot(T, obbA.axes[1]) * R[0][2] - glm::dot(T, obbA.axes[0]) * R[1][2]) > ra + rb) return false;

    return true;
}

bool Algorithms::SphereVsSphere(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB) {
    auto sphereA = std::static_pointer_cast<SphereShape>(entA.shapeNode->getShape())
                    ->BuildSphere(worldA);

    auto sphereB = std::static_pointer_cast<SphereShape>(entB.shapeNode->getShape())
                    ->BuildSphere(worldB);

    const float distanceSq = glm::distance2(sphereA.center, sphereB.center);
    const float radiusSum = sphereA.radius + sphereB.radius;
    const float radiusSumSq = radiusSum * radiusSum;

    return distanceSq <= radiusSumSq;
}

bool Algorithms::BoxVsSphere(const CollisionEntry& entBox, const CollisionEntry& entSphere, const glm::mat4& worldBox, const glm::mat4& worldSphere) {
    auto box = std::static_pointer_cast<BoxShape>(entBox.shapeNode->getShape())
                ->BuildOBB(worldBox);

    auto sphere = std::static_pointer_cast<SphereShape>(entSphere.shapeNode->getShape())
                   ->BuildSphere(worldSphere);

    const glm::vec3 relCenter = sphere.center - box.center;
    const auto localCenter = glm::vec3(
        glm::dot(relCenter, box.axes[0]),
        glm::dot(relCenter, box.axes[1]),
        glm::dot(relCenter, box.axes[2])
    );

    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(localCenter.x, -box.halfExtents.x, box.halfExtents.x);
    closestPoint.y = glm::clamp(localCenter.y, -box.halfExtents.y, box.halfExtents.y);
    closestPoint.z = glm::clamp(localCenter.z, -box.halfExtents.z, box.halfExtents.z);

    const float distanceSq = glm::distance2(closestPoint, localCenter);

    return distanceSq <= (sphere.radius * sphere.radius);
}

bool Algorithms::SphereVsCapsule(const CollisionEntry& entSphere, const CollisionEntry& entCapsule, const glm::mat4& worldSphere, const glm::mat4& worldCapsule) {
    auto sphere = std::static_pointer_cast<SphereShape>(entSphere.shapeNode->getShape())
                   ->BuildSphere(worldSphere);

    auto capsule = std::static_pointer_cast<CapsuleShape>(entCapsule.shapeNode->getShape())
                    ->BuildCapsule(worldCapsule);

    const glm::vec3 closestPoint = ClosestPointOnSegment(capsule.p0, capsule.p1, sphere.center);
    const float distSq = glm::distance2(sphere.center, closestPoint);
    const float radiusSum = sphere.radius + capsule.radius;

    return distSq <= (radiusSum * radiusSum);
}

bool Algorithms::CapsuleVsCapsule(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB) {
    auto capA = std::static_pointer_cast<CapsuleShape>(entA.shapeNode->getShape())
                 ->BuildCapsule(worldA);

    auto capB = std::static_pointer_cast<CapsuleShape>(entB.shapeNode->getShape())
                 ->BuildCapsule(worldB);

    glm::vec3 c1, c2;
    ClosestPointsTwoSegments(capA.p0, capA.p1, capB.p0, capB.p1, c1, c2);
    const float distSq = glm::distance2(c1, c2);
    const float radiusSum = capA.radius + capB.radius;

    return distSq <= (radiusSum * radiusSum);
}

bool Algorithms::BoxVsCapsule(const CollisionEntry& entBox, const CollisionEntry& entCapsule, const glm::mat4& worldBox, const glm::mat4& worldCapsule) {
    auto box = std::static_pointer_cast<BoxShape>(entBox.shapeNode->getShape())
                ->BuildOBB(worldBox);

    auto capsule = std::static_pointer_cast<CapsuleShape>(entCapsule.shapeNode->getShape())
                    ->BuildCapsule(worldCapsule);
    
    // Or use the OBB data we have:
    auto toLocal = [&](glm::vec3 worldP) {
        const glm::vec3 rel = worldP - box.center;
        return glm::vec3(
            glm::dot(rel, box.axes[0]),
            glm::dot(rel, box.axes[1]),
            glm::dot(rel, box.axes[2])
        );
    };

    glm::vec3 localP0 = toLocal(capsule.p0);
    glm::vec3 localP1 = toLocal(capsule.p1);
    glm::vec3 closestToCenter = ClosestPointOnSegment(localP0, localP1, glm::vec3(0.0f));
    glm::vec3 closestOnBox = glm::clamp(closestToCenter, -box.halfExtents, box.halfExtents);
    glm::vec3 closestOnSegmentLocal = ClosestPointOnSegment(localP0, localP1, closestOnBox);
    glm::vec3 finalClosestOnBox = glm::clamp(closestOnSegmentLocal, -box.halfExtents, box.halfExtents);
    
    float distSq = glm::distance2(closestOnSegmentLocal, finalClosestOnBox);

    return distSq <= (capsule.radius * capsule.radius);
}

bool Algorithms::SphereVsCylinder(const CollisionEntry& entSphere, const CollisionEntry& entCylinder, const glm::mat4& worldSphere, const glm::mat4& worldCylinder) {
    auto sphere = std::static_pointer_cast<SphereShape>(entSphere.shapeNode->getShape())
                   ->BuildSphere(worldSphere);

    auto cylinder = std::static_pointer_cast<CylinderShape>(entCylinder.shapeNode->getShape())
                     ->BuildCylinder(worldCylinder);


    const glm::vec3 d = cylinder.p1 - cylinder.p0;
    const float hSq = glm::dot(d, d);
    const float t = glm::dot(sphere.center - cylinder.p0, d) / hSq;
    
    if (t < 0.0f || t > 1.0f) {
        // Sphere is beyond the flat ends. Check distance to the end disks.
        // For a true cylinder, this is more complex, but Sphere vs Disk is:
        const glm::vec3 endPoint = (t < 0.0f) ? cylinder.p0 : cylinder.p1;
        const glm::vec3 rel = sphere.center - endPoint;
        const float distToPlane = glm::abs(glm::dot(rel, glm::normalize(d)));
        if (distToPlane > sphere.radius) return false;

        const float planarDistSq = glm::length2(rel - glm::dot(rel, glm::normalize(d)) * glm::normalize(d));
        if (planarDistSq > cylinder.radius * cylinder.radius) {
            // Closest point is on the edge of the disk
            const float planarDist = glm::sqrt(planarDistSq);
            const float distToEdgeSq = glm::pow(planarDist - cylinder.radius, 2.0f) + distToPlane * distToPlane;
            return distToEdgeSq <= (sphere.radius * sphere.radius);
        }
        return true;
    }

    // Sphere is between the end planes
    float distSqToAxis = glm::distance2(sphere.center, cylinder.p0 + t * d);

    return distSqToAxis <= (sphere.radius + cylinder.radius) * (sphere.radius + cylinder.radius);
}

bool Algorithms::BoxVsCylinder(const CollisionEntry& entBox, const CollisionEntry& entCylinder, const glm::mat4& worldBox, const glm::mat4& worldCylinder) {
    auto box = std::static_pointer_cast<BoxShape>(entBox.shapeNode->getShape())
                ->BuildOBB(worldBox);

    auto cylinder = std::static_pointer_cast<CylinderShape>(entCylinder.shapeNode->getShape())
                     ->BuildCylinder(worldCylinder);

    auto toLocal = [&](const glm::vec3 worldP) {
        const glm::vec3 rel = worldP - box.center;
        return glm::vec3(
            glm::dot(rel, box.axes[0]),
            glm::dot(rel, box.axes[1]),
            glm::dot(rel, box.axes[2])
        );
    };

    glm::vec3 localP0 = toLocal(cylinder.p0);
    glm::vec3 localP1 = toLocal(cylinder.p1);

    glm::vec3 closestToCenter = ClosestPointOnSegment(localP0, localP1, glm::vec3(0.0f));
    glm::vec3 closestOnBox = glm::clamp(closestToCenter, -box.halfExtents, box.halfExtents);
    glm::vec3 closestOnSegmentLocal = ClosestPointOnSegment(localP0, localP1, closestOnBox);
    glm::vec3 finalClosestOnBox = glm::clamp(closestOnSegmentLocal, -box.halfExtents, box.halfExtents);

    float distSq = glm::distance2(closestOnSegmentLocal, finalClosestOnBox);

    return distSq <= cylinder.radius * cylinder.radius;
}

bool Algorithms::CapsuleVsCylinder(const CollisionEntry& entCapsule, const CollisionEntry& entCylinder, const glm::mat4& worldCapsule, const glm::mat4& worldCylinder) {
    auto cap = std::static_pointer_cast<CapsuleShape>(entCapsule.shapeNode->getShape())
                  ->BuildCapsule(worldCapsule);
    
    auto cyl = std::static_pointer_cast<CylinderShape>(entCylinder.shapeNode->getShape())
                  ->BuildCylinder(worldCylinder);

    glm::vec3 c1, c2;
    ClosestPointsTwoSegments(cap.p0, cap.p1, cyl.p0, cyl.p1, c1, c2);
    const float distSq = glm::distance2(c1, c2);
    const float radiusSum = cap.radius + cyl.radius;

    return distSq <= radiusSum * radiusSum;
}

bool Algorithms::CylinderVsCylinder(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB) {
    auto cylA = std::static_pointer_cast<CylinderShape>(entA.shapeNode->getShape())
                 ->BuildCylinder(worldA);

    auto cylB = std::static_pointer_cast<CylinderShape>(entB.shapeNode->getShape())
                 ->BuildCylinder(worldB);

    glm::vec3 c1, c2;
    ClosestPointsTwoSegments(cylA.p0, cylA.p1, cylB.p0, cylB.p1, c1, c2);
    const float distSq = glm::distance2(c1, c2);
    const float radiusSum = cylA.radius + cylB.radius;

    return distSq <= radiusSum * radiusSum;
}
