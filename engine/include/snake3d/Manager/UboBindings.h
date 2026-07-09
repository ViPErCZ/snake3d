#ifndef SNAKE3_UBOBINDINGS_H
#define SNAKE3_UBOBINDINGS_H

#include <stdafx.h>

namespace Manager {

    // UBO binding-point conventions for the engine. Shader-side blocks
    // (e.g. `uniform FrameData { ... }`) are wired to these indices
    // post-link via glUniformBlockBinding (GL 3.3 doesn't support
    // `layout(binding=N)` for UBOs).
    inline constexpr GLuint UBO_BINDING_FRAME = 0;
    inline constexpr GLuint UBO_BINDING_MATERIAL = 1;
    inline constexpr GLuint UBO_BINDING_PARTICLES = 2;

} // namespace Manager

#endif //SNAKE3_UBOBINDINGS_H
