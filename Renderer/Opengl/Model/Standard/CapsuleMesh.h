#ifndef CAPSULEMESH_H
#define CAPSULEMESH_H

#include "StandardMesh.h"

namespace Model {
    class CapsuleMesh final : public StandardMesh {
    public:
        explicit CapsuleMesh(shared_ptr<ShaderManager> baseShader,
                             float height = 2.0, float radius = 0.5, int rings = 8, int segments = 64);
    };
} // Model

#endif //CAPSULEMESH_H
