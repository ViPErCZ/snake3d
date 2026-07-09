#ifndef SNAKE3_LIGHTNODE3D_H
#define SNAKE3_LIGHTNODE3D_H

#include <memory>

#include <snake3d/Renderer/Opengl/Model/Standard/WireframeArrowMesh.h>

namespace Model {
    class LightNode3D {
    public:
        explicit LightNode3D(const std::shared_ptr<Manager::ShaderProgram> &baseShader);

        virtual ~LightNode3D() = default;

    protected:
        glm::mat4 calculateArrowTransform(glm::vec3 position, glm::vec3 direction);

        std::shared_ptr<WireframeArrowMesh> arrowMesh;
    };
} // Model

#endif //SNAKE3_LIGHTNODE3D_H
