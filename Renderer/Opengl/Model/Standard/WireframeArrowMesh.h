#ifndef SNAKE3_WIREFRAMEARROWMESH_H
#define SNAKE3_WIREFRAMEARROWMESH_H

#include "StandardMesh.h"

namespace Model {
    class WireframeArrowMesh final : public StandardMesh {
    public:
        explicit WireframeArrowMesh(std::shared_ptr<Manager::ShaderProgram> baseShader);

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
            const glm::mat4 &parentTransform, bool shadows) const override;

        void setColor(const glm::vec3 color) { this->color = color; }

    protected:
        glm::vec3 color = glm::vec3{1.0f};
    };
} // Model

#endif //SNAKE3_WIREFRAMEARROWMESH_H