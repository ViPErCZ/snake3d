#ifndef CAPSULEMESH_H
#define CAPSULEMESH_H

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    class CapsuleMesh final : public StandardMesh {
    public:
        explicit CapsuleMesh(std::shared_ptr<Manager::ShaderProgram> baseShader,
                             float height = 2.0, float radius = 0.5, int rings = 8, int segments = 64);
    };
} // Model

#endif //CAPSULEMESH_H
