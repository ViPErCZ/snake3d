#ifndef SNAKE3_CLEAR_H
#define SNAKE3_CLEAR_H

#include <GL/glew.h>

namespace Tools {
    enum class Clear {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT,
        ColorDepth = Color | Depth,
        ColorDepthStencil = ColorDepth | Stencil
    };
}

#endif //SNAKE3_CLEAR_H