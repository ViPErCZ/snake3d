#ifndef SNAKE3_ARRAYMESH_H
#define SNAKE3_ARRAYMESH_H

#include "StandardMesh.h"

using namespace std;

namespace Model {
    class ArrayMesh final : public StandardMesh {
    public:
        explicit ArrayMesh(const shared_ptr<ShaderProgram> &baseShader) : StandardMesh(baseShader) {}

        void fromMesh(const shared_ptr<Mesh> &mesh);

        void fromVertexData(vector<Vertex> &vertices);
    };
} // Model

#endif //SNAKE3_ARRAYMESH_H
