#ifndef SNAKE3_LIGHTNODE3D_H
#define SNAKE3_LIGHTNODE3D_H

#include <memory>

#include "../Standard/WireframeArrowMesh.h"

namespace Model {
    class LightNode3D {
    public:
        explicit LightNode3D(const shared_ptr<ShaderProgram> &baseShader);

        virtual ~LightNode3D() = default;

    protected:
        glm::mat4 calculateArrowTransform(glm::vec3 position, glm::vec3 direction);

        shared_ptr<WireframeArrowMesh> arrowMesh;
    };
} // Model

#endif //SNAKE3_LIGHTNODE3D_H
