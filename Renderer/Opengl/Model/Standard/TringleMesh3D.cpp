#include "TringleMesh3D.h"

using namespace Manager;
using namespace ModelUtils;
using namespace std;

namespace Model {
    TringleMesh3D::TringleMesh3D(shared_ptr<ShaderProgram> baseShader, const float width, const float height)
        : StandardMesh(std::move(baseShader)) {
        std::vector<Vertex> vertices(3);

        float halfW = width / 2.0f;
        float halfH = height / 2.0f;

        // Jednoduchý rovnostranný-ish trojúhelník okolo středu
        vertices[0].position = { 0.0f,   halfH, 0.0f };   // Top
        vertices[1].position = { halfW, -halfH, 0.0f };   // Bottom-Right
        vertices[2].position = { -halfW,-halfH, 0.0f };   // Bottom-Left

        vertices[0].texUV = {0.5f, 1.0f};   // horní střed tex
        vertices[1].texUV = {1.0f, 0.0f};   // pravý spodní
        vertices[2].texUV = {0.0f, 0.0f};   // levý spodní

        std::vector<GLuint> indices = {0, 1, 2};

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model