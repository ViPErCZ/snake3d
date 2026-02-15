#ifndef SNAKE3_BOXSHAPE_H
#define SNAKE3_BOXSHAPE_H

#include "Shape.h"
#include <vector>

#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;

namespace Physic {
    class BoxShape : public Shape {
        struct OBB {
            glm::vec3 center; // Střed boxu ve světě
            glm::vec3 axes[3]; // Normalizované směrové vektory (Right, Up, Forward)
            glm::vec3 halfExtents; // Poloviční velikost boxu (započítaný scale)
        };

    public:
        explicit BoxShape(
            const shared_ptr<ResourceManager> &resourceManager,
            const shared_ptr<ContextState> &contextState,
            glm::vec3 boxSize = glm::vec3(1.0f)
        );

        ShapeType getType() override { return ShapeType::Box; }

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;

        [[nodiscard]] OBB BuildOBB(const glm::mat4 &modelMatrix) const;

        AABB calculateAABB(const glm::mat4 &modelMatrix) override;

        [[nodiscard]] const shared_ptr<MeshNode3D> &getMeshNode() const { return meshNode; }

    private:

        glm::vec3 size;

        shared_ptr<MeshNode3D> meshNode;

        shared_ptr<StandardMaterial> material;
    };
} // Physic

#endif //SNAKE3_BOXSHAPE_H
