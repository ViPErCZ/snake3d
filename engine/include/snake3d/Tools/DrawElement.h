#ifndef SNAKE3_DRAWELEMENT_H
#define SNAKE3_DRAWELEMENT_H

#include <GL/gl.h>

namespace Tools {
    enum class DrawElement {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN
    };
}

#endif //SNAKE3_DRAWELEMENT_H