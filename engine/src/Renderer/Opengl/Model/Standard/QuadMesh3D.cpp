#include <snake3d/Renderer/Opengl/Model/Standard/QuadMesh3D.h>

using namespace Manager;
using namespace ModelUtils;
using namespace std;

namespace Model {
    QuadMesh3D::QuadMesh3D(shared_ptr<ShaderProgram> baseShader, const float width, const float height)
        : StandardMesh(std::move(baseShader)) {
        std::vector<Vertex> vertices(4);

        float halfW = width / 2.0f;
        float halfH = height / 2.0f;

        vertices[0].position = { -halfW, 0.0f,  halfH }; // Top-Left
        vertices[1].position = {  halfW, 0.0f,  halfH }; // Top-Right
        vertices[2].position = {  halfW, 0.0f, -halfH }; // Bottom-Right
        vertices[3].position = { -halfW, 0.0f, -halfH }; // Bottom-Left

        vertices[0].texUV = { 0.0f, 1.0f }; // Top-Left
        vertices[1].texUV = { 1.0f, 1.0f }; // Top-Right
        vertices[2].texUV = { 1.0f, 0.0f }; // Bottom-Right
        vertices[3].texUV = { 0.0f, 0.0f }; // Bottom-Left

        std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
        };

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model
