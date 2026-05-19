#ifndef CYLINDERMESH_H
#define CYLINDERMESH_H

#include "StandardMesh.h"

using namespace std;

namespace Model {
    class CylinderMesh final : public StandardMesh {
    public:
        CylinderMesh(shared_ptr<ShaderProgram> baseShader,
                     float topRadius, float bottomRadius,
                     float height, int rings, int segments);
    };
} // Model

#endif //CYLINDERMESH_H
