#ifndef SNAKE3_DEPTHFUNC_H
#define SNAKE3_DEPTHFUNC_H

namespace Tools {
    enum class DepthFunc {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        Lequal = GL_LEQUAL,
        Greater = GL_GREATER,
        Notequal = GL_NOTEQUAL,
        Gequal = GL_GEQUAL,
        Always = GL_ALWAYS
    };

    enum class DepthMask {
        True = GL_TRUE,
        False = GL_FALSE
    };
} // Tools

#endif //SNAKE3_DEPTHFUNC_H