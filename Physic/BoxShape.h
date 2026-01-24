#ifndef SNAKE3_BOXSHAPE_H
#define SNAKE3_BOXSHAPE_H

#include "Shape.h"

namespace Physic {
    class BoxShape : public Shape {
        struct OBB {
            glm::vec3 center;       // Střed boxu ve světě
            glm::vec3 axes[3];      // Normalizované směrové vektory (Right, Up, Forward)
            glm::vec3 halfExtents;  // Poloviční velikost boxu (započítaný scale)
        };
    public:
        OBB BuildOBB(const glm::mat4& modelMatrix, const glm::vec3& originalBoxSize);
    };
} // Physic

#endif //SNAKE3_BOXSHAPE_H