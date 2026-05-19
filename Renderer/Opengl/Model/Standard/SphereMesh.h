#ifndef SNAKE3_SPHEREMESH_H
#define SNAKE3_SPHEREMESH_H

#include "StandardMesh.h"

using namespace Manager;
using namespace ModelUtils;
using namespace std;


namespace Model {
    class SphereMesh final : public StandardMesh {
    public:
        explicit SphereMesh(shared_ptr<ShaderProgram> baseShader, float height = 1.0,
                           float radius = 0.5, int rings = 32, int segments = 64);
    };
} // Model

#endif //SNAKE3_SPHEREMESH_H