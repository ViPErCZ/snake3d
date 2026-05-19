#ifndef BOX_MESH_H
#define BOX_MESH_H

#include "StandardMesh.h"

using namespace std;

namespace Model {
    class BoxMesh final : public StandardMesh {
        float depth;
    public:
        explicit BoxMesh(shared_ptr<ShaderProgram> baseShader, float width, float height, float depth);
    };
} // Model

#endif //BOX_MESH_H
