#ifndef SNAKE3_TRINGLENODE3D_H
#define SNAKE3_TRINGLENODE3D_H

#include "StandardMesh.h"

namespace Model {
    class TringleMesh3D : public StandardMesh {
    public:
        explicit TringleMesh3D(shared_ptr<ShaderManager> baseShader, float width, float height);
    };
} // Model

#endif //SNAKE3_TRINGLENODE3D_H