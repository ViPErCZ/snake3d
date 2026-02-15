#ifndef SNAKE3_SPHERESHAPE_H
#define SNAKE3_SPHERESHAPE_H

#include "Shape.h"

#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;

namespace Physic {
    class SphereShape : public Shape {
    public:
        struct SphereWorldData {
            glm::vec3 center;
            float radius;
        };

    private:
        float radius;
        
        shared_ptr<MeshNode3D> meshNode;

        shared_ptr<StandardMaterial> material;

    public:
        explicit SphereShape(const shared_ptr<ResourceManager> &resourceManager,
            const shared_ptr<ContextState> &contextState, float radius = 1.0f);

        ShapeType getType() override { return ShapeType::Sphere; }

        [[nodiscard]] float getRadius() const { return radius; }

        [[nodiscard]] SphereWorldData BuildSphere(const glm::mat4& modelMatrix) const;

        AABB calculateAABB(const glm::mat4& modelMatrix) override {
            auto [center, radius] = BuildSphere(modelMatrix);
            return {
                center - glm::vec3(radius),
                center + glm::vec3(radius)
            };
        }

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;
    };
} // Physic

#endif //SNAKE3_SPHERESHAPE_H