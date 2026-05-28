#ifndef SNAKE3_SHAPE_H
#define SNAKE3_SHAPE_H

#include <vector>

#include <snake3d/Tools/Transform.h>
#include "Manager/Camera.h"
#include <snake3d/Tools/BuildSettings.h>

namespace Model {
    class MeshNode3D;
}

namespace Physic {

    enum class ShapeType {
        Box, Sphere, Capsule, Cylinder,
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    class Shape {
    public:
        virtual ~Shape() = default;

        virtual ShapeType getType() = 0;
        virtual void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, glm::mat4 t) = 0;
        virtual AABB calculateAABB(const glm::mat4& modelMatrix) = 0;

        void setColliding(const bool colliding) { this->colliding = colliding; }
        [[nodiscard]] bool isColliding() const { return colliding; }
        void setCollisionEnabled(const bool enabled) { collisionEnabled = enabled; }
        [[nodiscard]] bool isCollisionEnabled() const { return collisionEnabled; }

        // Model:: is required !!!
        [[nodiscard]] const std::shared_ptr<Model::MeshNode3D> &getMeshNode() const { return meshNode; }

        static AABB CalculateAABB(const glm::mat4& modelMatrix, const glm::vec3& localHalfExtents) {

            const auto center = glm::vec3(modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            const glm::vec3 right   = glm::abs(glm::vec3(modelMatrix[0]));
            const glm::vec3 up      = glm::abs(glm::vec3(modelMatrix[1]));
            const glm::vec3 forward = glm::abs(glm::vec3(modelMatrix[2]));

            const glm::vec3 newHalfExtents =
                  right   * localHalfExtents.x
                + up      * localHalfExtents.y
                + forward * localHalfExtents.z;

            return { center - newHalfExtents, center + newHalfExtents };
        }

        static std::vector<glm::vec3> GetAABBCorners(const AABB& aabb) {
            std::vector<glm::vec3> corners(8);

            // Spodní stěna (Y = min.y)
            corners[0] = glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z); // min-min-min (Levý-Dolní-Zadní)
            corners[1] = glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z); // max-min-min
            corners[2] = glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z); // max-min-max
            corners[3] = glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z); // min-min-max

            // Horní stěna (Y = max.y)
            corners[4] = glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z); // min-max-min
            corners[5] = glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z); // max-max-min
            corners[6] = glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z); // max-max-max
            corners[7] = glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z); // min-max-max

            return corners;
        }

    protected:
        std::shared_ptr<Model::MeshNode3D> meshNode; // Model:: is required !!!
    private:
        bool colliding = false;
        bool collisionEnabled = true;
    };
} // Physic

#endif //SNAKE3_SHAPE_H
