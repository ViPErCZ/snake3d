#ifndef SNAKE3_STANDARDMESH_H
#define SNAKE3_STANDARDMESH_H

#include <memory>
#include "../../../../ItemsDto/BaseItem.h"
#include "../Utils/Mesh.h"

using namespace ModelUtils;

namespace Model {
    class StandardMesh {
    public:
        StandardMesh(std::shared_ptr<BaseItem> baseItem, float width, float height);
        [[nodiscard]] std::shared_ptr<Mesh> getMesh() const;
        [[nodiscard]] std::shared_ptr<BaseItem> getBaseItem() const;

    protected:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<BaseItem> item;
        float width, height;
    };
} // Model

#endif //SNAKE3_STANDARDMESH_H