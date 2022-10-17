#include "Vbo.h"

namespace ModelUtils {
    Vbo::Vbo(vector<Vertex> &vertices) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, (long)(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    }

    void Vbo::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
    }

    void Vbo::unBind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Vbo::clear() {
        glDeleteBuffers(1, &ID);
    }
} // ModelUtils