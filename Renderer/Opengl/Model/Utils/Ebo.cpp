#include "Ebo.h"

namespace ModelUtils {
    Ebo::Ebo(vector<GLuint> &indices) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(indices.size() * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);
    }

    void Ebo::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    }

    void Ebo::unBind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Ebo::clear() {
        glDeleteBuffers(1, &ID);
    }
} // ModelUtils