#ifndef SNAKE3_QUADMESH3D_H
#define SNAKE3_QUADMESH3D_H

#include "StandardMesh.h"

namespace Model {
    class QuadMesh3D : public StandardMesh {
    public:
        explicit QuadMesh3D(shared_ptr<ShaderManager> baseShader, float width, float height);
    };
} // Model

#endif //SNAKE3_QUADMESH3D_H