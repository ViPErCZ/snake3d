#include <snake3d/Renderer/Opengl/Model/Utils/Mesh2D.h>

using namespace std;

namespace ModelUtils {
    Mesh2D::Mesh2D(const vector<Vertex2D> &vertices, const vector<unsigned int> &indices, string name)
        : vertices(vertices), indices(indices), name(std::move(name)) {

        vao = nullptr;
    }

    Mesh2D::~Mesh2D() {
        delete vao;
    }

    const vector<GLuint> &Mesh2D::getIndices() const {
        return indices;
    }

    const vector<Vertex2D> &Mesh2D::getVertices() const {
        return vertices;
    }

    void Mesh2D::bind() {
        initialize();
        vao->bind();
    }

    const glm::mat4 &Mesh2D::getGlobalTransformation() const {
        return globalTransformation;
    }

    void Mesh2D::setGlobalTransformation(const glm::mat4 &globalTransformation) {
        Mesh2D::globalTransformation = globalTransformation;
    }

    const string &Mesh2D::getName() const {
        return name;
    }

    void Mesh2D::initialize() {
        if (vao != nullptr) {
            return;
        }

        vao = new Vao();
        vao->bind();
        // Generates Vertex Buffer Object and links it to vertices
        Vbo vbo(this->vertices);
        // Generates Element Buffer Object and links it to indices
        Ebo ebo(this->indices);
        // Links VBO attributes such as coordinates and colors to VAO
        vao->linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, position)));
        vao->linkAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, texUV)));
        vao->linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, color)));
        // Unbind all to prevent accidentally modifying them
        vao->unBind();
        Vbo::unBind();
        ebo.unBind();
    }

} // ModelUtils