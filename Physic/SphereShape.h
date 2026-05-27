#ifndef SNAKE3_SPHERESHAPE_H
#define SNAKE3_SPHERESHAPE_H

#include "Shape.h"

#include "../Renderer/Opengl/Material/MaterialInstance.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

namespace Physic {
    class SphereShape : public Shape {
    public:
        struct SphereWorldData {
            glm::vec3 center;
            float radius;
        };

    private:
        float radius;
        std::shared_ptr<Material::MaterialInstance> material;
        std::shared_ptr<Feature::AlbedoFeature> albedoFeature;
        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<Manager::ResourceManager> resourceManager;

    public:
        explicit SphereShape(const std::shared_ptr<Manager::ResourceManager> &resourceManager,
            const std::shared_ptr<Tools::ContextState> &contextState, float radius = 1.0f);

        ShapeType getType() override { return ShapeType::Sphere; }

        [[nodiscard]] float getRadius() const { return radius; }

        void setRadius(float radius);

        [[nodiscard]] SphereWorldData BuildSphere(const glm::mat4& modelMatrix) const;

        AABB calculateAABB(const glm::mat4& modelMatrix) override {
            auto [center, radius] = BuildSphere(modelMatrix);
            return {
                center - glm::vec3(radius),
                center + glm::vec3(radius)
            };
        }

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;
    };
} // Physic

#endif //SNAKE3_SPHERESHAPE_H