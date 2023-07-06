#include "Vao.h"

namespace ModelUtils {
    Vao::Vao() {
        glGenVertexArrays(1, &ID);
    }

    void Vao::linkAttrib(Vbo &VBO, GLuint layout, int numComponents, GLenum type, int stride, void *offset) const {
        VBO.bind();
        glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        VBO.unBind();
    }

    void Vao::linkAttribI(Vbo &VBO, GLuint layout, int numComponents, GLenum type, int stride, void *offset) const {
        VBO.bind();
        glVertexAttribIPointer(layout, numComponents, type, stride, offset);
        glEnableVertexAttribArray(layout);
        VBO.unBind();
    }

    void Vao::bind() const {
        glBindVertexArray(ID);
    }

    void Vao::unBind() const {
        glBindVertexArray(0);
    }

    void Vao::clear() {
        glDeleteVertexArrays(1, &ID);
    }
} // ModelUtils