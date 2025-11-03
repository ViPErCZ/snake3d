#ifndef SNAKE3_ARRAYMESH_H
#define SNAKE3_ARRAYMESH_H

#include "StandardMesh.h"
#include "../../../../ItemsDto/ObjItem.h"

using namespace std;

namespace Model {
    class ArrayMesh final : public StandardMesh {
    public:
        explicit ArrayMesh(const shared_ptr<ShaderManager> &baseShader) : StandardMesh(baseShader) {}

        void fromObj(const shared_ptr<ObjItem> &item);

        void fromMesh(const shared_ptr<Mesh> &mesh);

        void fromVertexData(vector<Vertex> &vertices);
    };
} // Model

#endif //SNAKE3_ARRAYMESH_H
