#include "Ebo.h"

using namespace std;

namespace ModelUtils {
    Ebo::Ebo(const vector<GLuint> &indices) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);
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