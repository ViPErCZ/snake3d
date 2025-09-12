#ifndef SNAKE3_STANDARDMESH_H
#define SNAKE3_STANDARDMESH_H

#include <memory>
#include "../../../../ItemsDto/BaseItem.h"
#include "../Utils/Mesh.h"
#include "../../Material/StandardMaterial.h"
#include "../../../../Manager/ShaderManager.h"

using namespace ModelUtils;
using namespace Material;
using namespace std;

namespace Model {
    class StandardMesh {
    public:
        StandardMesh(shared_ptr<BaseItem> baseItem, shared_ptr<ShaderManager> baseShader, float width,
                     float height);

        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

        [[nodiscard]] shared_ptr<BaseItem> getBaseItem() const;

        void setMaterial(const shared_ptr<BaseMaterial> &material);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const;
        void renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const;

        [[nodiscard]] glm::vec3 getMin() const;

        [[nodiscard]] glm::vec3 getMax() const;

    protected:
        shared_ptr<Mesh> mesh;
        shared_ptr<BaseItem> item;
        shared_ptr<BaseMaterial> material;
        shared_ptr<ShaderManager> baseShader;
        float width, height;
        glm::vec3 localMin;
        glm::vec3 localMax;
    };
} // Model

#endif //SNAKE3_STANDARDMESH_H
