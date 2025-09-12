#ifndef SNAKE3_ARRAYMESH_H
#define SNAKE3_ARRAYMESH_H

#include "StandardMesh.h"
#include "../../../../ItemsDto/ObjItem.h"

using namespace std;

namespace Model {
    class ArrayMesh : public StandardMesh {
    public:
        ArrayMesh(const shared_ptr<BaseItem> &baseItem, const shared_ptr<ShaderManager> &baseShader)
            : StandardMesh(baseItem, baseShader, 0, 0) {
        }

        void fromObj(const shared_ptr<ObjItem> &item);

        void fromMesh(const shared_ptr<Mesh> &mesh);

        void fromVertexData(vector<Vertex> &vertices);
    };
} // Model

#endif //SNAKE3_ARRAYMESH_H
