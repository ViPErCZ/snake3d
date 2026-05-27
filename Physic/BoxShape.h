#ifndef SNAKE3_BOXSHAPE_H
#define SNAKE3_BOXSHAPE_H

#include "Shape.h"

#include "../Renderer/Opengl/Material/MaterialInstance.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

namespace Physic {
    class BoxShape : public Shape {
        struct OBB {
            glm::vec3 center; // Střed boxu ve světě
            glm::vec3 axes[3]; // Normalizované směrové vektory (Right, Up, Forward)
            glm::vec3 halfExtents; // Poloviční velikost boxu (započítaný scale)
        };

    public:
        explicit BoxShape(
            const std::shared_ptr<Manager::ResourceManager> &resourceManager,
            const std::shared_ptr<Tools::ContextState> &contextState,
            glm::vec3 boxSize = glm::vec3(1.0f)
        );

        ShapeType getType() override { return ShapeType::Box; }

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, glm::mat4 t) override;

        [[nodiscard]] OBB BuildOBB(const glm::mat4 &modelMatrix) const;

        AABB calculateAABB(const glm::mat4 &modelMatrix) override;

        [[nodiscard]] const glm::vec3 &getSize() const { return size; }

        void setSize(const glm::vec3 &size);

    private:

        glm::vec3 size;

        std::shared_ptr<Material::MaterialInstance> material;
        std::shared_ptr<Feature::AlbedoFeature> albedoFeature;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Tools::ContextState> contextState;
    };
} // Physic

#endif //SNAKE3_BOXSHAPE_H
