#include "CollisionAlgorithms.h"

#include "../BoxShape.h"


bool Algorithms::BoxVsBox(const CollisionEntry &entA, const CollisionEntry &entB) {
    const auto obbA = std::static_pointer_cast<BoxShape>(
        entA.shapeNode->getShape())->BuildOBB(entA.parentObject->getModelMatrix() * entA.shapeNode->getModelMatrix()
    );
    const auto obbB = std::static_pointer_cast<BoxShape>(
        entB.shapeNode->getShape())->BuildOBB(entB.parentObject->getModelMatrix() * entB.shapeNode->getModelMatrix()
    );

    glm::vec3 T = obbB.center - obbA.center;
    T = glm::vec3(glm::dot(T, obbA.axes[0]), glm::dot(T, obbA.axes[1]), glm::dot(T, obbA.axes[2]));

    glm::mat3 R;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = glm::dot(obbA.axes[i], obbB.axes[j]);

    glm::mat3 AbsR;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            AbsR[i][j] = glm::abs(R[i][j]) + 1e-6f;

    float ra, rb;

    for (int i = 0; i < 3; i++) {
        ra = obbA.halfExtents[i];
        rb = obbB.halfExtents[0] * AbsR[i][0] + obbB.halfExtents[1] * AbsR[i][1] + obbB.halfExtents[2] * AbsR[i][2];
        if (glm::abs(T[i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; i++) {
        ra = obbA.halfExtents[0] * AbsR[0][i] + obbA.halfExtents[1] * AbsR[1][i] + obbA.halfExtents[2] * AbsR[2][i];
        rb = obbB.halfExtents[i];
        if (glm::abs(T[0] * R[0][i] + T[1] * R[1][i] + T[2] * R[2][i]) > ra + rb) return false;
    }

    ra = obbA.halfExtents[1] * AbsR[2][0] + obbA.halfExtents[2] * AbsR[1][0];
    rb = obbB.halfExtents[1] * AbsR[0][2] + obbB.halfExtents[2] * AbsR[0][1];
    if (glm::abs(T[2] * R[1][0] - T[1] * R[2][0]) > ra + rb) return false;

    return true;
}

// bool Physic::Algorithms::SphereVsSphere(const CollisionEntry& a, const CollisionEntry& b) {
//     auto sphereA = std::static_pointer_cast<SphereShape>(a.shapeNode->getShape())->BuildSphere(a.parentObject->getModelMatrix() * a.shapeNode->getModelMatrix());
//     auto sphereB = std::static_pointer_cast<SphereShape>(b.shapeNode->getShape())->BuildSphere(b.parentObject->getModelMatrix() * b.shapeNode->getModelMatrix());
//
//     float distanceSq = glm::distance2(sphereA.center, sphereB.center); // Používáme čtverec vzdálenosti (rychlejší - bez odmocniny)
//     float radiusSum = sphereA.radius + sphereB.radius;
//
//     return distanceSq <= (radiusSum * radiusSum);
// }
