#ifndef SNAKE3_CAPSULESHAPE_H
#define SNAKE3_CAPSULESHAPE_H

#include "Shape.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;

namespace Physic {
    class CapsuleShape : public Shape {
    public:
        struct CapsuleWorldData {
            glm::vec3 p0; // Start point of the segment (bottom center)
            glm::vec3 p1; // End point of the segment (top center)
            float radius;
        };

    private:
        float radius;
        float height; // Total height including hemispherical ends

        shared_ptr<MeshNode3D> meshNode;
        shared_ptr<StandardMaterial> material;

    public:
        explicit CapsuleShape(const shared_ptr<ResourceManager> &resourceManager,
                             const shared_ptr<ContextState> &contextState, 
                             float radius = 0.5f, float height = 2.0f);

        ShapeType getType() override { return ShapeType::Capsule; }

        [[nodiscard]] float getRadius() const { return radius; }
        [[nodiscard]] float getHeight() const { return height; }

        [[nodiscard]] CapsuleWorldData BuildCapsule(const glm::mat4& modelMatrix) const;

        AABB calculateAABB(const glm::mat4& modelMatrix) override;

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;
    };
} // Physic

#endif //SNAKE3_CAPSULESHAPE_H
