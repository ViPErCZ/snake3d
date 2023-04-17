#ifndef SNAKE3_MESHMODEL_H
#define SNAKE3_MESHMODEL_H

#include "Utils/Mesh.h"

using namespace Manager;
using namespace ModelUtils;

namespace Model {

    class MeshModel {
    public:
        explicit MeshModel(BaseItem *baseItem);
        virtual ~MeshModel();
        [[nodiscard]] Mesh *getMesh() const;

    protected:
        Mesh* mesh;
        BaseItem* item;
    };

} // Model

#endif //SNAKE3_MESHMODEL_H
