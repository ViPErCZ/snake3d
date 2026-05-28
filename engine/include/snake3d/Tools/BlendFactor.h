#ifndef SNAKE3_BLEND_FACTOR_H
#define SNAKE3_BLEND_FACTOR_H

#include <GL/gl.h>

namespace Tools {
    enum class BlendFactor {
        None,
        Zero = GL_ZERO,
        One = GL_ONE,
        DstColor = GL_DST_COLOR,
        SrcColor = GL_SRC_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        BlendColor = GL_CONSTANT_COLOR
    };
}
#endif