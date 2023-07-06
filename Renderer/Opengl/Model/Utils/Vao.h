#ifndef SNAKE3_VAO_H
#define SNAKE3_VAO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Vbo.h"

namespace ModelUtils {

    class Vao {
    public:
        // ID reference for the Vertex Array Object
        GLuint ID{};
        // Constructor that generates a VAO ID
        Vao();
        // Links a VBO Attribute such as a position or color to the VAO
        void linkAttrib(Vbo& VBO, GLuint layout, int numComponents, GLenum type, int stride, void* offset) const;
        void linkAttribI(Vbo &VBO, GLuint layout, int numComponents, GLenum type, int stride, void *offset) const;
        // Binds the VAO
        void bind() const;
        // Unbinds the VAO
        void unBind() const;
        // Deletes the VAO
        void clear();
    };

} // ModelUtils

#endif //SNAKE3_VAO_H
