#ifndef SNAKE3_CYLINDERSHAPE_H
#define SNAKE3_CYLINDERSHAPE_H

#include "Shape.h"
#include "../Renderer/Opengl/Material/MaterialInstance.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;

namespace Physic {
    class CylinderShape : public Shape {
    public:
        struct CylinderWorldData {
            glm::vec3 p0; // Spodní střed
            glm::vec3 p1; // Horní střed
            float radius;
        };

    private:
        float radius;
        float height;
        shared_ptr<Material::MaterialInstance> material;
        shared_ptr<Feature::AlbedoFeature> albedoFeature;
        shared_ptr<ContextState> contextState;
        shared_ptr<ResourceManager> resourceManager;

    public:
        explicit CylinderShape(const shared_ptr<ResourceManager> &resourceManager,
                               const shared_ptr<ContextState> &contextState,
                               float radius = 1.0f, float height = 2.0f);

        ShapeType getType() override { return ShapeType::Cylinder; }

        [[nodiscard]] CylinderWorldData BuildCylinder(const glm::mat4 &modelMatrix) const;

        AABB calculateAABB(const glm::mat4 &modelMatrix) override;

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;

        [[nodiscard]] float getRadius() const { return radius; }

        [[nodiscard]] float getHeight() const { return height; }

        void setRadius(float radius);

        void setHeight(float height);
    };
} // Physic

#endif //SNAKE3_CYLINDERSHAPE_H
