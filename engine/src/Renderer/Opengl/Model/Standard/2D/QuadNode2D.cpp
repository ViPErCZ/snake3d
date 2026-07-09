#include <snake3d/Renderer/Opengl/Model/Standard/2D/QuadNode2D.h>

using namespace std;
using namespace ModelUtils;
using namespace Manager;

namespace Model {
    QuadNode2D::QuadNode2D(const float width, const float height, const shared_ptr<ShaderProgram> &baseShader) : BaseNode2D(baseShader) {
        std::vector<Vertex2D> vertices(4);

        float halfW = width / 2.0f;
        float halfH = height / 2.0f;

        vertices[0].position = {-halfW, -halfH,  0.0f};
        vertices[1].position = { halfW, -halfH,  0.0f};
        vertices[2].position = { halfW,  halfH,  0.0f};
        vertices[3].position = {-halfW,  halfH,  0.0f};

        vertices[0].texUV = {0.0f, 0.0f};
        vertices[1].texUV = {1.0f, 0.0f};
        vertices[2].texUV = {1.0f, 1.0f};
        vertices[3].texUV = {0.0f, 1.0f};

        std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

        mesh = std::make_shared<Mesh2D>(vertices, indices);
    }
} // Model
