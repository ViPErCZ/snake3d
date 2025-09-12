#ifndef CAPSULEMESH_H
#define CAPSULEMESH_H

#include "SphereMesh.h"

namespace Model {
    class CapsuleMesh : public StandardMesh {
    public:
        explicit CapsuleMesh(shared_ptr<BaseItem> baseItem, shared_ptr<ShaderManager> baseShader,
                             float height = 2.0, float radius = 0.5, int rings = 8,
                             int segments = 64);
    };
} // Model

#endif //CAPSULEMESH_H
