#include "BoxShape.h"

namespace Physic {
    BoxShape::BoxShape(const glm::vec3 boxSize) : size(boxSize) {}

    void BoxShape::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const glm::mat4 t) {
    //     // 1. Spočítáme aktuální OBB
    //     OBB currentOBB = BuildOBB(t);
    //
    //     // 2. Získáme 8 rohů ve světě
    //     std::vector<glm::vec3> c = GetOBBCorners(currentOBB);
    //
    //     // Barva (Cyan jako v Godotu)
    //     glm::vec3 color(0.0f, 1.0f, 1.0f);
    //
    //     // 3. Vykreslíme 12 hran (Wireframe)
    //     // Spodek
    //     DrawDebugLine(c[0], c[1], color);
    //     DrawDebugLine(c[1], c[2], color);
    //     DrawDebugLine(c[2], c[3], color);
    //     DrawDebugLine(c[3], c[0], color);
    //
    //     // Vršek
    //     DrawDebugLine(c[4], c[5], color);
    //     DrawDebugLine(c[5], c[6], color);
    //     DrawDebugLine(c[6], c[7], color);
    //     DrawDebugLine(c[7], c[4], color);
    //
    //     // Propojení spodku a vršku
    //     DrawDebugLine(c[0], c[4], color);
    //     DrawDebugLine(c[1], c[5], color);
    //     DrawDebugLine(c[2], c[6], color);
    //     DrawDebugLine(c[3], c[7], color);

        const auto aabb = CalculateAABB(t, this->size * 0.5f);
        const auto mvp = projection * camera->getViewMatrix();
        DrawAABB(aabb, mvp, isColliding() ? glm::vec3(0.8f, 0.5f, 0.5f) : glm::vec3(1.0f, 0.0f, 0.0f));
    }

    BoxShape::OBB BoxShape::BuildOBB(const glm::mat4& modelMatrix) {
        OBB obb{};

        obb.center = glm::vec3(modelMatrix[3]);

        glm::vec3 right   = glm::vec3(modelMatrix[0]);
        glm::vec3 up      = glm::vec3(modelMatrix[1]);
        glm::vec3 forward = glm::vec3(modelMatrix[2]);

        float scaleX = glm::length(right);
        float scaleY = glm::length(up);
        float scaleZ = glm::length(forward);

        obb.axes[0] = (scaleX > 0.0001f) ? right / scaleX : glm::vec3(1,0,0);
        obb.axes[1] = (scaleY > 0.0001f) ? up / scaleY : glm::vec3(0,1,0);
        obb.axes[2] = (scaleZ > 0.0001f) ? forward / scaleZ : glm::vec3(0,0,1);

        obb.halfExtents.x = (this->size.x * 0.5f) * scaleX;
        obb.halfExtents.y = (this->size.y * 0.5f) * scaleY;
        obb.halfExtents.z = (this->size.z * 0.5f) * scaleZ;

        return obb;
    }

    AABB BoxShape::calculateAABB(const glm::mat4 &modelMatrix) {
        return CalculateAABB(modelMatrix, this->size * 0.5f);
    }

    std::vector<glm::vec3> BoxShape::GetOBBCorners(const OBB &obb) const {
        std::vector<glm::vec3> corners(8);

        // Předvypočítáme si vektory os vynásobené poloměrem pro snazší sčítání
        glm::vec3 x = obb.axes[0] * obb.halfExtents.x;
        glm::vec3 y = obb.axes[1] * obb.halfExtents.y;
        glm::vec3 z = obb.axes[2] * obb.halfExtents.z;

        // Spodní část
        corners[0] = obb.center - x - y - z;
        corners[1] = obb.center + x - y - z;
        corners[2] = obb.center + x - y + z;
        corners[3] = obb.center - x - y + z;

        // Horní část
        corners[4] = obb.center - x + y - z;
        corners[5] = obb.center + x + y - z;
        corners[6] = obb.center + x + y + z;
        corners[7] = obb.center - x + y + z;

        return corners;
    }
} // Physic