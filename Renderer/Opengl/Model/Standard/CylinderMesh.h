#ifndef CYLINDERMESH_H
#define CYLINDERMESH_H

#include "StandardMesh.h"

namespace Model {
    class CylinderMesh final : public StandardMesh {
    public:
        CylinderMesh(std::shared_ptr<Manager::ShaderProgram> baseShader,
                     float topRadius, float bottomRadius,
                     float height, int rings, int segments);
    };
} // Model

#endif //CYLINDERMESH_H
