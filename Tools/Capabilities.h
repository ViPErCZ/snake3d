#ifndef SNAKE3_CAPABILITIES_H
#define SNAKE3_CAPABILITIES_H

#include <GL/gl.h>

namespace Tools {
    enum class Capabilities {
        DepthTest = GL_DEPTH_TEST,
        Blending = GL_BLEND,
        ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST,
        CullFace = GL_CULL_FACE
    };
}

#endif