#include "CollisionAlgorithms.h"

#include "../BoxShape.h"
#include "../SphereShape.h"

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

bool Algorithms::SphereVsSphere(const CollisionEntry& entA, const CollisionEntry& entB) {
    // 1. Získáme world data pro obě koule
    // BuildSphere už v sobě má započítaný scale objektu
    auto sphereA = std::static_pointer_cast<SphereShape>(entA.shapeNode->getShape())
                    ->BuildSphere(entA.parentObject->getModelMatrix() * entA.shapeNode->getModelMatrix());

    auto sphereB = std::static_pointer_cast<SphereShape>(entB.shapeNode->getShape())
                    ->BuildSphere(entB.parentObject->getModelMatrix() * entB.shapeNode->getModelMatrix());

    // 2. Vzdálenost mezi středy
    // Používáme GLM squared distance (distance2), protože odmocnina je drahá operace
    float distanceSq = glm::distance2(sphereA.center, sphereB.center);

    // 3. Součet poloměrů na druhou
    float radiusSum = sphereA.radius + sphereB.radius;
    float radiusSumSq = radiusSum * radiusSum;

    // Pokud je čtverec vzdálenosti menší než čtverec součtu poloměrů, je to zásah
    return distanceSq <= radiusSumSq;
}

bool Algorithms::BoxVsSphere(const CollisionEntry& entBox, const CollisionEntry& entSphere) {
    auto box = std::static_pointer_cast<BoxShape>(entBox.shapeNode->getShape())
                ->BuildOBB(entBox.parentObject->getModelMatrix() * entBox.shapeNode->getModelMatrix());

    auto sphere = std::static_pointer_cast<SphereShape>(entSphere.shapeNode->getShape())
                   ->BuildSphere(entSphere.parentObject->getModelMatrix() * entSphere.shapeNode->getModelMatrix());

    // 2. Převedeme střed koule do lokálního prostoru OBB
    const glm::vec3 relCenter = sphere.center - box.center;
    const auto localCenter = glm::vec3(
        glm::dot(relCenter, box.axes[0]),
        glm::dot(relCenter, box.axes[1]),
        glm::dot(relCenter, box.axes[2])
    );

    // 3. Najdeme nejbližší bod v Boxu k centru koule (Clamping)
    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(localCenter.x, -box.halfExtents.x, box.halfExtents.x);
    closestPoint.y = glm::clamp(localCenter.y, -box.halfExtents.y, box.halfExtents.y);
    closestPoint.z = glm::clamp(localCenter.z, -box.halfExtents.z, box.halfExtents.z);

    // 4. Vzdálenost mezi nejbližším bodem a středem koule
    const float distanceSq = glm::distance2(closestPoint, localCenter);

    // Pokud je vzdálenost menší než poloměr koule, došlo ke kolizi
    return distanceSq <= (sphere.radius * sphere.radius);
}
