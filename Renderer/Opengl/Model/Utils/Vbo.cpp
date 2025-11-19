#include "Vbo.h"

namespace ModelUtils {
    Vbo::Vbo(const vector<Vertex> &vertices) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    }

    Vbo::Vbo(const vector<Vertex2D> &vertices) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    }

    Vbo::Vbo() {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER,ID);
    }

    void Vbo::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
    }

    void Vbo::unBind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Vbo::clear() const {
        glDeleteBuffers(1, &ID);
    }
} // ModelUtils