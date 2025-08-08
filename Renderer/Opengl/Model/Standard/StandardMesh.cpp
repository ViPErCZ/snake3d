#include "StandardMesh.h"

namespace Model {
    StandardMesh::StandardMesh(std::shared_ptr<BaseItem> baseItem, const float width,
                               const float height) : item(std::move(baseItem)), width(width), height(height) {
    }

    std::shared_ptr<Mesh> StandardMesh::getMesh() const {
        return mesh;
    }

    std::shared_ptr<BaseItem> StandardMesh::getBaseItem() const {
        return item;
    }
} // Model
