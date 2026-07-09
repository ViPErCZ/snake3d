#ifndef SNAKE3_QUADMESH3D_H
#define SNAKE3_QUADMESH3D_H

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    class QuadMesh3D : public StandardMesh {
    public:
        explicit QuadMesh3D(std::shared_ptr<Manager::ShaderProgram> baseShader, float width, float height);
    };
} // Model

#endif //SNAKE3_QUADMESH3D_H