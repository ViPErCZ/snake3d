#include "TringleNode2D.h"

namespace Model {
    TringleNode2D::TringleNode2D(const float width, const float height, const shared_ptr<ShaderProgram> &baseShader)
        : BaseNode2D(baseShader) {
        std::vector<Vertex2D> vertices(3);

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

        mesh = std::make_shared<Mesh2D>(vertices, indices);
    }
} // Model