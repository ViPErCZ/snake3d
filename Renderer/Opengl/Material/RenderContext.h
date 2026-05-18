#ifndef SNAKE3_RENDERCONTEXT_H
#define SNAKE3_RENDERCONTEXT_H

#include <glm/glm.hpp>

namespace Material {
    // Per-draw kontext sdílený napříč features. Nahrazuje historickou
    // signaturu `bind(posView, view, projection, model, shadows)` jedním
    // strukturovaným parametrem.
    //
    // Vyrobí ho mesh renderer (typicky StandardMesh::render po B6) a předá
    // do `MaterialInstance::bind`. Features čtou jen pole která potřebují.
    struct RenderContext {
        glm::vec3 viewPos{0.0f};
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        glm::mat4 model{1.0f};
        float uTime = 0.0f;
        bool shadows = false;
    };
} // Material

#endif //SNAKE3_RENDERCONTEXT_H
