#ifndef BOX_MESH_H
#define BOX_MESH_H

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    class BoxMesh final : public StandardMesh {
        float depth;
    public:
        explicit BoxMesh(std::shared_ptr<Manager::ShaderProgram> baseShader, float width, float height, float depth);
    };
} // Model

#endif //BOX_MESH_H
