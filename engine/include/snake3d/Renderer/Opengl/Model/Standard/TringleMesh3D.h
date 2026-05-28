#ifndef SNAKE3_TRINGLENODE3D_H
#define SNAKE3_TRINGLENODE3D_H

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    class TringleMesh3D : public StandardMesh {
    public:
        explicit TringleMesh3D(std::shared_ptr<Manager::ShaderProgram> baseShader, float width, float height);
    };
} // Model

#endif //SNAKE3_TRINGLENODE3D_H