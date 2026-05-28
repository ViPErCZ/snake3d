#ifndef SNAKE3_ARRAYMESH_H
#define SNAKE3_ARRAYMESH_H

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    class ArrayMesh final : public StandardMesh {
    public:
        explicit ArrayMesh(const std::shared_ptr<Manager::ShaderProgram> &baseShader) : StandardMesh(baseShader) {}

        void fromMesh(const std::shared_ptr<ModelUtils::Mesh> &mesh);

        void fromVertexData(std::vector<ModelUtils::Vertex> &vertices);
    };
} // Model

#endif //SNAKE3_ARRAYMESH_H
