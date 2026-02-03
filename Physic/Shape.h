#ifndef SNAKE3_SHAPE_H
#define SNAKE3_SHAPE_H

#include <glm/gtc/type_ptr.hpp>

#include "../ItemsDto/Transform.h"

using namespace Node3D;

namespace Physic {

    enum class ShapeType {
        Sphere,
        Box
    };

    class Shape {
        struct AABB {
            glm::vec3 min;
            glm::vec3 max;
        };
    public:
        virtual ~Shape() = default;

        virtual ShapeType getType() = 0;
        virtual void render(Transform t) = 0;

        static AABB CalculateAABB(const glm::mat4& modelMatrix, const glm::vec3& localHalfExtents) {

            const auto center = glm::vec3(modelMatrix[3]);

            const glm::vec3 right   = glm::abs(glm::vec3(modelMatrix[0]));
            const glm::vec3 up      = glm::abs(glm::vec3(modelMatrix[1]));
            const glm::vec3 forward = glm::abs(glm::vec3(modelMatrix[2]));

            const glm::vec3 newHalfExtents =
                  right   * localHalfExtents.x
                + up      * localHalfExtents.y
                + forward * localHalfExtents.z;

            return { center - newHalfExtents, center + newHalfExtents };
        }
    };
} // Physic

#endif //SNAKE3_SHAPE_H