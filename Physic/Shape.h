#ifndef SNAKE3_SHAPE_H
#define SNAKE3_SHAPE_H

#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "../ItemsDto/Transform.h"
#include "../Manager/Camera.h"
#include "../Renderer/Opengl/Line.h"

using namespace Manager;
using namespace Node3D;

namespace Physic {

    enum class ShapeType {
        Sphere,
        Box
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    class Shape {
    public:
        virtual ~Shape() = default;

        virtual ShapeType getType() = 0;
        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) = 0;
        virtual AABB calculateAABB(const glm::mat4& modelMatrix) = 0;

        void setColliding(const bool colliding) { this->colliding = colliding; }
        [[nodiscard]] bool isColliding() const { return colliding; }

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

        static vector<glm::vec3> GetAABBCorners(const AABB& aabb) {
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

        static void DrawAABB(const AABB& aabb, glm::mat4 mvp, glm::vec3 color) {
            auto corners = GetAABBCorners(aabb);

            // Definice párů indexů, které tvoří čáry
            int indices[] = {
                0,1, 1,2, 2,3, 3,0, // Spodek
                4,5, 5,6, 6,7, 7,4, // Vršek
                0,4, 1,5, 2,6, 3,7  // Sloupky
            };

            // Vykreslení 12 čar
            for (int i = 0; i < 24; i += 2) {
                glm::vec3 start = corners[indices[i]];
                glm::vec3 end   = corners[indices[i+1]];

                // Tvoje funkce pro vykreslení čáry ve 3D
                Line line(start, end);
                line.setColor(color);
                line.setMVP(mvp);
                line.draw();
            }
        }

    private:
        bool colliding = false;
    };
} // Physic

#endif //SNAKE3_SHAPE_H