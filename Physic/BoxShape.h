#ifndef SNAKE3_BOXSHAPE_H
#define SNAKE3_BOXSHAPE_H

#include "Shape.h"
#include <vector>

namespace Physic {
    class BoxShape : public Shape {
        struct OBB {
            glm::vec3 center; // Střed boxu ve světě
            glm::vec3 axes[3]; // Normalizované směrové vektory (Right, Up, Forward)
            glm::vec3 halfExtents; // Poloviční velikost boxu (započítaný scale)
        };

    public:
        explicit BoxShape(glm::vec3 boxSize = glm::vec3(1.0f));

        ShapeType getType() override { return ShapeType::Box; }

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;

        OBB BuildOBB(const glm::mat4 &modelMatrix);

        AABB calculateAABB(const glm::mat4& modelMatrix) override;

    private:
        std::vector<glm::vec3> GetOBBCorners(const OBB &obb) const;

        glm::vec3 size;
    };
} // Physic

#endif //SNAKE3_BOXSHAPE_H
