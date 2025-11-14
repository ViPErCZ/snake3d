#ifndef SNAKE3_BASENODE2D_H
#define SNAKE3_BASENODE2D_H

#include <memory>

#include "../../../../../Manager/Camera.h"
#include "../../../../../Manager/ShaderManager.h"
#include "../../../Material/BaseMaterial.h"
#include "../../Utils/Mesh.h"

using namespace std;
using namespace ModelUtils;
using namespace Manager;
using namespace Material;

namespace Model {
    class BaseNode2D {
    public:
        explicit BaseNode2D(const shared_ptr<ShaderManager> &baseShader);

        virtual ~BaseNode2D() = default;

        void setMaterial(const shared_ptr<BaseMaterial> &material);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                            const glm::mat4 &parentTransform) const;

        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

        virtual void update(float dt) {
        };

    protected:
        shared_ptr<Mesh> mesh;
        shared_ptr<BaseMaterial> material;
        shared_ptr<ShaderManager> baseShader;
        unsigned int textureId = 0;
    };
} // Model

#endif //SNAKE3_BASENODE2D_H
